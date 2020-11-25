/*****************************************************************
 * Copyright (c)2020, by gpf/lzy
 * All rights reserved.
 * FileName：		server.cpp
 * System：          	Windows
 * SubSystem：        	Common System Service
 * Author：		gpf/lzy
 * Date：		2020.11.24
 * Version：		1.0
 * Description：
		。Hub类 方法的具体实现
			。构造函数
			。析构函数
			。接受用户请求
			。处理用户请求
 *
 * Last Modified:

	2020.11.24, 	By lzy
*/


#define _WINSOCK_DEPRECATED_NO_WARNINGS // ignore inet_ntoa errors
#define _CRT_SECURE_NO_WARNINGS					// ignore strcpy errors

#include "server.h"
#include "mystring.h"
#include <conio.h>
#include <thread>
#include <iostream>
#include <map>
#include <windows.h>

using namespace std;

Function_pool::Function_pool() : m_function_queue(), m_lock(), m_data_condition(), m_accept_functions(true)
{
}

Function_pool::~Function_pool()
{
}

void Function_pool::push(std::function<void()> func)
{
	std::unique_lock<std::mutex> lock(m_lock);
	m_function_queue.push(func);
	// when we send the notification immediately, the consumer will try to get the lock , so unlock asap
	lock.unlock();
	m_data_condition.notify_one();
}

void Function_pool::done()
{
	std::unique_lock<std::mutex> lock(m_lock);
	m_accept_functions = false;
	lock.unlock();
	// when we send the notification immediately, the consumer will try to get the lock , so unlock asap
	m_data_condition.notify_all();
	//notify all waiting threads.
}

void Function_pool::infinite_loop_func()
{
	std::function<void()> func;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(m_lock);
			m_data_condition.wait(lock, [this]() {return !m_function_queue.empty() || !m_accept_functions; });
			if (!m_accept_functions && m_function_queue.empty())
			{
				//lock will be release automatically.
				//finish the thread loop and let it join in the main thread.
				return;
			}
			func = m_function_queue.front();
			m_function_queue.pop();
			//release the lock
		}
		func();
	}
}

//inline ThreadPool::ThreadPool(unsigned int num)
//	: _thread_num(num), stop(false)
//{
//	for (unsigned int i = 0; i < _thread_num; ++i)
//		workers.emplace_back(
//			[this]
//			{
//				std::cout << endl;
//				std::cout << "begin this thread " << std::this_thread::get_id() << " : " << std::endl;//返回当前线程的id号
//				for (;;)
//				{
//					std::function<void()> task;
//
//					{
//						//声明一个锁管理类实例，在其全生命周期内占有这个锁，配合cv变量来进行进程的同步
//						std::unique_lock<std::mutex> lock(this->queue_mutex);
//						//利用cv变量来同步
//						//只有当 pred 条件为 false 时调用 wait() 才会阻塞当前线程，并且在收到其他线程的通知后只有当 pred 为 true 时才会被解除阻塞。
//						//if task not empty must finish the task whether thread pool is running or not
//						this->condition.wait(lock,
//							[this] { return this->stop || !this->tasks.empty(); });//如果this->stop == true 或者 队列不为空就不阻塞.
//						if (this->stop && this->tasks.empty())//如果任务队列为空并且threadpool停止运行，就返回
//							return;
//						task = std::move(this->tasks.front());
//						this->tasks.pop();
//					}
//				}
//				std::cout << "end this thread " << std::this_thread::get_id() << " : " << std::endl;
//			}
//			);
//}
//
//// add new work item to the pool
//template<class F, class... Args>
//auto ThreadPool::enqueue(F&& f, Args&&... args)
//-> std::future<typename std::result_of<F(Args...)>::type>
//{
//	using return_type = typename std::result_of<F(Args...)>::type;
//
//	auto task = std::make_shared< std::packaged_task<return_type()> >(
//		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
//		);
//	
//	std::future<return_type> res = task->get_future();
//
//	{
//		std::unique_lock<std::mutex> lock(queue_mutex);
//
//		// don't allow enqueueing after stopping the pool
//		if (stop)
//			throw std::runtime_error("enqueue on stopped ThreadPool");
//
//		tasks.emplace([task]() { (*task)(); });
//	}
//	condition.notify_one();
//	return res;
//}


///////////////////////////////////////////////////////////////////// 		
// Function：		
//		getInstance                            	
// Description:                                                       		
//		static function to get the instance of the class Hub                                         		                                                   			
// Return Values:                                                     		
//      Hub&                                              		
/////////////////////////////////////////////////////////////////////	
Hub& Hub::getInstance()
{
	static Hub result;
	return result;
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		~Hub()                            	
// Description:                                                       		
//		The deconstructor function of the class Hub                                         		                                                   			
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
Hub::~Hub()
{
#pragma region delete endpoints
	// all endpoints should be destroyed in Hub::start()
	mtx.lock();
	while (endpoints.size())
	{
		auto p = endpoints.back();
		delete p;
		endpoints.pop_back();
	}
	//threadPool.~ThreadPool();
	mtx.unlock();
#pragma endregion

	//closesocket(hubSocket); // if socket has been closed, return WSAENOTSOCK, but that's ok
	//closesocket(connSocket);

	/**
	 * WSACleanup, stop socket DLL
	 * - return 0 if WSACleanup has NOT been called(succeed)
	 * - return -1 if WSACleanup has been called
	*/
	while (WSACleanup() != -1)
		;
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		start()                            	
// Description:                                                       		
//		start the hub to Init the database
//		data persistence
//	
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
void Hub::start()
{
#pragma region open database
	cout << "Hub: Init database...";
	// open the data base if not exsits then print error information
	if (sqlite3_open("server.db", &db))
	{
		cout << "\nHub: Can NOT open database: " << sqlite3_errmsg(db) << endl;
		return;
	}
	//Create Table User and Pokemon
	char* errMsg;
	string sql = "create table User(";
	sql += "id integer primary key,";
	sql += "name text unique not null,";
	sql += "password text not null,";
	sql += "win int not null,";
	sql += "total int not null";
	sql += ");";
	if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
	{
		sqlite3_free(errMsg);
	}
	sql = "create table Pokemon(";
	sql += "id integer primary key,";
	sql += "userid integer not null,";
	sql += "name text not null,";
	sql += "race int not null,";
	sql += "atk int not null,";
	sql += "def int not null,";
	sql += "maxHp int not null,";
	sql += "speed int not null,";
	sql += "lv int not null,";
	sql += "exp int not null";
	sql += ");";
	if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
	{
		sqlite3_free(errMsg);
	}
	cout << "Done.\n";
#pragma endregion

	// initialization socket DLL
	cout << "Hub: Init socket DLL...";
	WSADATA wsadata;
	// WSAStartup函数被用来初始化Socket环境，主版本号为2，副版本号为2
	if (WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		cout << "\nHub: Init network protocol failed.\n";
		return;
	}
	cout << "Done.\n";

#pragma region init socket
	/**
	 * init hub socket
	 * function: socket(int domain, int type, int protocol);
	 * domain: AF_INET or PF_INET
	 *   - AF for Address Family
	 *   - PF for Protocol Family
	 *   - in Windows, AF_INET == PF_INET
	 * type: SOCK_STREAM or SOCK_DGRAM or SOCK_RAW
	 * protocol: use IPPROTO_TCP for TCP/IP
	*/
	cout << "Hub: Init hub socket...";
	hubSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//初始化hub的socket

	if (hubSocket == INVALID_SOCKET)
	{
		cout << "\nHub: Init socket failed.\n";
		closesocket(hubSocket);
		WSACleanup();
		// system("pause");
		return;
	}
	cout << "Done.\n";

	// construct an socket, which include protocol type, listening port, listening ip address
	sockaddr_in hubAddr;
	hubAddr.sin_family = AF_INET;
	hubAddr.sin_port = htons(HUB_PORT);
	hubAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // any ip address

	// bind socket to an address
	cout << "Hub: Socket binding...";
	// as a server, It should bind the address and port to the server
	if (::bind(hubSocket, (sockaddr*)&hubAddr, sizeof(hubAddr)) == SOCKET_ERROR)
	{
		cout << "\nHub: Socket bind failed.\n";
		closesocket(hubSocket);
		WSACleanup();
		return;
	}
	cout << "Done.\n";

	// the limit of the listening queue is req_queue_length, if request queue is full, client will get error: WSAECONNREFUSED
	cout << "Hub: Socket listen...";
	if (listen(hubSocket, REQ_QUEUE_LENGTH) == SOCKET_ERROR)
	{
		cout << WSAGetLastError();
		cout << "\nHub: Socket listen failed.\n";
		closesocket(hubSocket);
		WSACleanup();
		return;
	}
	cout << "Done.\n";
#pragma endregion

	// now listening successfully
	cout << "\nHub: Hub is running at " << HUB_PORT << endl;


	
	// init thread
	running = true;
	//std::cout << "stating operation" << std::endl;
	int num_threads = std::thread::hardware_concurrency();
	std::cout << "number of threads = " << num_threads << std::endl;
	for (int i = 0; i < num_threads; i++)
	{
		thread_pool.push_back(std::thread(&Function_pool::infinite_loop_func, &threadPool));
	}

	//here we should send our functions
	threadPool.push(std::bind(&Hub::listenFunc, this));
	threadPool.push(std::bind(&Hub::terminateFunc, this));
	threadPool.done();
	for (unsigned int i = 0; i < thread_pool.size(); i++)
	{
		thread_pool.at(i).join();
	}
	//thread listenThread(&Hub::listenFunc, this);
	//thread terminateThread(&Hub::terminateFunc, this);
	//listenThread.join();
	//terminateThread.join();
	
}


///////////////////////////////////////////////////////////////////// 		
// Function：		
//		listenFunc()                            	
// Description:                                                       		
//		listen the clients' requests
//		recv and process the login information
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
void Hub::listenFunc()//hub的监听函数
{

	while (running)
	{
		// link
		sockaddr_in clientAddr; // client address
		int clientAddrLength = sizeof(clientAddr);
		connSocket = accept(hubSocket, (sockaddr*)&clientAddr, &clientAddrLength);
		if (connSocket == INVALID_SOCKET)
		{
			if (running)
			{
				// if not running, this thread must be terminated by terminateFunc
				// in that case the string below is not needed
				cout << "Hub: Link to client failed.\n";
			}
			closesocket(connSocket);
			break;
		}
		

		// link successfully
		cout << "Hub: " << inet_ntoa(clientAddr.sin_addr) << " connected.\n";//将网络地址转换成“.”点隔的字符串格式

		/**
		 * process data
		 * format:
		 * - "login\n<username>\n<password>"
		 * - "logon\n<username>\n<password>"
		*/
		recv(connSocket, buf, BUF_LENGTH, 0);
		auto strs = split(buf);
		if (strs.size() == 1 && strs[0].length() == 0)
		{
			//空白请求，可能client已经关闭了
		}
		else if (strs.size() < 3)
		{
			//error
			cout << "Hub: Invalid request: " << buf << endl;
			strcpy(buf, "Reject: Invalid request.\n");
			send(connSocket, buf, BUF_LENGTH, 0);
		}
		else if (strs[0] == "login")
			login(strs[1], strs[2]);
		else if (strs[0] == "logon")
			logon(strs[1], strs[2]);
		else
		{
			cout << "Hub: Invalid request: " << buf << endl;
			strcpy(buf, "Reject: Invalid request.\n");
			send(connSocket, buf, BUF_LENGTH, 0);
		}
		closesocket(connSocket);
	}
}


///////////////////////////////////////////////////////////////////// 		
// Function：		
//		terminateFunc()                            	
// Description:                                                       		
//		terminate the void function
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
void Hub::terminateFunc()
{
	// press any key to stop server
	cout << "Press any key to stop hub.\n\n";
	_getch();
	running = false;
	mtx.lock();
	while (endpoints.size())
	{
		auto p = endpoints.back();
		delete p;
		endpoints.pop_back();
	}
	mtx.unlock();

	closesocket(hubSocket);
	WSACleanup();

	sqlite3_close(db);

	cout << "\nHub: Hub stoped.\n";
}


///////////////////////////////////////////////////////////////////// 		
// Function：		
//		login()                            	
// Description:                                                       		
//		login the game
// Args:
//		const string&	username
//		const string&	password
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
void Hub::login(const string& username, const string& password)
{	
	//username有问题
	if (!isValidUsername(username))
	{
		cout << "Hub: Got an invalid username: " << username << endl;
		strcpy(buf, "Reject: Invalid username.\n");
	}
	else if (!isValidPassword(password))//password有问题
	{
		cout << "Hub: Got an invalid password: " << password << endl;
		strcpy(buf, "Reject: Invalid password.\n");
	}
	else//username和password在数据库查询
	{
		//选择数据库失败
		char** sqlResult;
		int nRow;
		int nColumn;
		char* errMsg;
		string sql = "SELECT id FROM User WHERE name = '" + username + "' AND password = '" + password + "'";
		if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
		{
			cout << "Hub: Sqlite3 error: " << errMsg << endl;
			// strcpy(buf, "Reject: Hub database error.\n");
			strcpy(buf, "服务器数据库错误");
			sqlite3_free(errMsg);
		}
		else//sqlite选择成功
		{	//查询不到该用户
			if (nRow == 0)
			{
				// username and password mismatch
				cout << "Hub: Login: username '" << username << "' and password '" << password << "' mismatch.\n";
				// strcpy(buf, "Reject: Username and password dismatch.\n");
				strcpy(buf, "用户名或密码错误");
			}
			else
			{
				// username存在
				// 检查该用户的状态
				bool userExist = false;
				mtx.lock();
				int id = atoi(sqlResult[1]); // sqlResult[0] == "id", sqlResult[1] == playerID
				for (auto endpoint : endpoints)//查enpoints中的每一个enpoints线程中有无该用户的id
				{
					if (endpoint->getPlayerID() == id)
					{
						userExist = true;
						if (endpoint->isOnline())
						{
							// strcpy(buf, "Reject: Account is already online.\n");
							strcpy(buf, "用户已在其他设备登录");
						}
						else
						{
							// 说明不在线，返回端口号
							strcpy(buf, to_string(endpoint->getPort()).c_str());
						}
						break;
					}
				}
				mtx.unlock();

				if (!userExist) // 用户状态不在线，则增加一个endpoint
				{
					auto p = new Endpoint(id, db, *this);
					int endpointPort = p->start();//启动一个新的endpoint(),并返回一个端口号
					if (endpointPort == 0) //启动失败，移除并删除该端口
					{
						delete p;
						// strcpy(buf, "Reject: Hub endpoint error.\n");
						strcpy(buf, "服务器错误");
					}
					else //启动正常，将这个endpoint添加到endpoints中
					{
						lock_guard<mutex> lock(mtx);
						endpoints.push_back(p);
						strcpy(buf, to_string(endpointPort).c_str());
						//here we should send our functions
						//threadPool.push(std::bind(&Hub::mornitor, this, p));
						//threadPool.done();
						//thread_pool.at(thread_pool.size()).detach();

						//threadPool.enqueue(&Hub::mornitor, this, p);
						thread th(&Hub::mornitor, this, p);//增加一个用于处理该endpoint的mornitor线程
						th.detach();
					}
				}
			}
			sqlite3_free_table(sqlResult);
		}
	}
	send(connSocket, buf, BUF_LENGTH, 0);
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		logon()                            	
// Description:                                                       		
//		logon the username and the password
// Args:
//		const string&	username
//		const string&	password
// Return Values:                                                     		
//      None                                              		
/////////////////////////////////////////////////////////////////////	
void Hub::logon(const string& username, const string& password)//注册用户
{
	if (!isValidUsername(username))
	{
		cout << "Hub: Got an invalid username: " << username << endl;
		// strcpy(buf, "Reject: Invalid username.\n");
		strcpy(buf, "不合法的用户名");
	}
	else if (!isValidPassword(password))
	{
		cout << "Hub: Got an invalid password: " << password << endl;
		// strcpy(buf, "Reject: Invalid password.\n");
		strcpy(buf, "不合法的密码");
	}
	else
	{
		char** sqlResult;
		int nRow;
		int nColumn;
		char* errMsg;
		string sql = "SELECT name FROM User WHERE name = '" + username + "'";
		if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
		{
			cout << "Hub: Sqlite3 error: " << errMsg << endl;
			// strcpy(buf, "Reject: Hub database error.\n");
			strcpy(buf, "服务器数据库错误");
			sqlite3_free(errMsg);
		}
		else
		{
			if (nRow == 0)
			{
				// username NOT exist, add this user
				string sql = "INSERT INTO User(name, password, win, total) VALUES('" + username + "', '" + password + "', 0, 0);";
				char* errMsg;
				if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
				{
					cout << "Hub: Sqlite3 error: " << errMsg << endl;
					sqlite3_free(errMsg);
					strcpy(buf, "服务器数据库错误");
					// strcpy(buf, "Reject: Hub database error.\n");
				}
				else
				{
					cout << "Hub: Add user: " << username << " password: " << password << endl;
					strcpy(buf, "Accept.\n");
				}
			}
			else
			{
				// username already exist
				cout << "Hub: Logon: username '" << username << "' already exist.\n";
				//strcpy(buf, "Reject: Duplicate username.\n");
				strcpy(buf, "用户名已存在");
			}
			sqlite3_free_table(sqlResult);
		}
	}
	send(connSocket, buf, BUF_LENGTH, 0);
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		isValidUsername()                            	
// Description:                                                       		
//		check whether the username is valid
// Args:
//		const string&	str
// Return Values:                                                     		
//      bool                                               		
/////////////////////////////////////////////////////////////////////	
bool Hub::isValidUsername(const string& str)
{
	for (auto c : str)
	{
		if (c == ' ' || c == '\n' || c == '\t')
		{
			// contains blank \n \t
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		isValidPassword()                            	
// Description:                                                       		
//		check whether the password is valid
//		the pass word should be consists of alpha and digit and underline
// Args:
//		const string&	str
// Return Values:                                                     		
//      bool                                               		
/////////////////////////////////////////////////////////////////////
bool Hub::isValidPassword(const string& str)
{
	for (auto c : str)
	{
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'))
		{
			//not a letter or a digit or '_'
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////// 		
// Function：		
//		mornitor()                            	
// Description:                                                       		
//		monitor the endpoint
//		force the endpoint to start
// Args:
//		Endpoint* 
// Return Values:                                                     		
//      bool                                               		
/////////////////////////////////////////////////////////////////////
void Hub::mornitor(Endpoint* const endpoint)
{
	endpoint->process();

	// now endpoint reaches end
	mtx.lock();
	// remove from endpoints
	for (int i = 0; i < endpoints.size(); ++i)
	{
		if (endpoints[i] == endpoint)
		{
			endpoints.erase(endpoints.begin() + i);
			// if endpoints doesn't contain endpoint, that means endpoint has been deleted in ~Hub()
			delete endpoint;
			break;
		}
	}
	mtx.unlock();
}


///////////////////////////////////////////////////////////////////// 		
// Function：		
//		getAllUser()                            	
// Description:                                                       		
//		get the user list in the database
// Return Values:                                                     		
//      string format:
//					<userID> <userName> <online:0 | 1> <win> <total>
/////////////////////////////////////////////////////////////////////
string Hub::getAllUser()
{
	struct temp	//创建一个temp结构存信息，格式如上
	{
		string name;
		bool online;
		string win;
		string total;
	};
	// get user from database
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql = "SELECT id, name, win, total FROM User;";
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Hub: Sqlite3 error: " << errMsg << endl;
		// strcpy(buf, "Reject: Hub database error.\n");
		// strcpy(buf, "服务器数据库错误");
		sqlite3_free(errMsg);
	}

	// 先创建一个包含每个玩家的map
	map<int, temp> playerMap;
	for (int i = 0; i < nRow; ++i)//nRow条信息
	{
		temp t = { sqlResult[4 * (i + 1) + 1], false, sqlResult[4 * (i + 1) + 2], sqlResult[4 * (i + 1) + 3] };
		playerMap.insert(make_pair(stoi(sqlResult[4 * (i + 1)]), t));
	}

	sqlite3_free_table(sqlResult);

	// 再判断每个玩家是否在线
	string result;
	mtx.lock();
	for (auto endpoint : endpoints)
	{
		playerMap[endpoint->getPlayerID()].online = true;
	}
	mtx.unlock();

	// 优先显示在线玩家的信息
	// 序列化玩家信息
	for (auto& player : playerMap)
	{
		if (player.second.online)
		{
			// result = onlinePlayer + result;
			result = to_string(player.first) + ' ' + player.second.name + " 1 " + player.second.win + " " + player.second.total + "\n" + result;
		}
		else
		{
			// result = result + onlinePlayer
			result += to_string(player.first) + ' ' + player.second.name + " 0 " + player.second.win + " " + player.second.total + "\n";
		}
	}

	return result;
}
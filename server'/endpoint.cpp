#define _CRT_SECURE_NO_WARNINGS // ignore strcpy errors

#include "endpoint.h"
#include <iostream>
// #include <mstcpip.h> // for socket keep_alive
#include "mystring.h"
#include "server.h"
#include "battle.h"

using namespace std;

///////////////////////////////////////////////////////////////////// 		
// function��		
//		Endpoint()���캯��
// args:
//		playerID, db, hub
// Description:                                                       		
//		����ID����
/////////////////////////////////////////////////////////////////////
Endpoint::Endpoint(int playerID, sqlite3*& db, Hub& hub) : playerID(playerID), db(db), hub(hub)
{
	port = 0;
	running = false;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		~Endpoint()
// Description:                                                       		
//		�����������ͷ�
/////////////////////////////////////////////////////////////////////
Endpoint::~Endpoint()
{
	running = false;
	while (timing)
	{
		// cout << "Try to stop timer.\n";
		unique_lock<mutex> lock(mtx);
		online = true;
		lock.unlock();
		cv.notify_one();
		lock.lock();
	}

	closesocket(endpointSocket);

	if (port)
		cout << "Endpoint[" << playerID << "]: Endpoint stoped at " << port << endl;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		start()
// Description:                                                       		
//		��֤��¼��Ϣ����ѯ��
/////////////////////////////////////////////////////////////////////
int Endpoint::start()
{
	// get playerUsername
	char** sqlResult;
	int nRow;//��ѯ����������¼
	int nColumn;//���ٸ��ֶΣ������У�
	char* errMsg; /* Error msg written here */
	string sql = "SELECT name FROM User where id=" + to_string(playerID) + ";";//sql���,����playerID��ѯUser���е�name
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		return 0;
	}
	else if (nRow == 0)//nrow=0˵����0����Ϣ��˵�����ݿ��в�ѯ����
	{
		cout << "Endpoint[" << playerID << "]: Database content error.\n";
		sqlite3_free_table(sqlResult);
		return 0;
	}
	else
	{
		playerUsername = sqlResult[1];
		sqlite3_free_table(sqlResult);
	}

	/**
	 * init endpoint socket
	 *
	 * function: socket(int domain, int type, int protocol);
	 * domain: AF_INET or PF_INET
	 *   - AF for Address Family
	 *   - PF for Protocol Family
	 *   - in Windows, AF_INET == PF_INET
	 * type: SOCK_STREAM or SOCK_DGRAM or SOCK_RAW
	 * protocol: use IPPROTO_TCP for TCP/IP
	*/
	// cout << "Endpoint[" << playerID << "]: init socket...";

	endpointSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����һ��endpointsocket
	if (endpointSocket == INVALID_SOCKET)//��Ϊ��Ч�׽��֣��򴴽�ʧ��
	{
		cout << "Endpoint[" << playerID << "]: Init socket failed.\n";
		closesocket(endpointSocket);
		// system("pause");
		return 0;
	}
	// cout << "Done.\n";

	// construct an address, including protocol & IP address & port
	//������ַ�����а�����Э�顢IP��ַ�Ͷ˿ں�
	sockaddr_in endpointAddr;//�������ݱ��ĵ�ַ
	endpointAddr.sin_family = AF_INET;
	endpointAddr.sin_port = htons(0); // port = 0 so windows will give us a free port
	endpointAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // any ip address



	// bind socket to an address
	//��socket�󶨵�һ����ַ��
	// cout << "Endpoint[" << playerID << "]: Socket binding...";
	if (::bind(endpointSocket, (sockaddr*)&endpointAddr, sizeof(endpointAddr)) == SOCKET_ERROR)
	{
		cout << "Endpoint[" << playerID << "]: Socket bind failed.\n";
		closesocket(endpointSocket);
		// system("pause");
		return 0;
	}
	// cout << "Done.\n";

	// ϵͳ������һ�����еĶ˿�
	int endpointAddrLength = sizeof(endpointAddr);
	getsockname(endpointSocket, (sockaddr*)&endpointAddr, &endpointAddrLength);
	port = ntohs(endpointAddr.sin_port);

	//�������������ˣ��ͻ��˽��ᱨ��:WSAECONNREFUSED
	// cout << "Endpoint[" << playerID << "]: Socket listen...";
	if (listen(endpointSocket, REQ_QUEUE_LENGTH) == SOCKET_ERROR)//����ʧ��
	{
		cout << WSAGetLastError();
		cout << "Endpoint[" << playerID << "]: Socket listen failed.\n";
		closesocket(endpointSocket);
		// system("pause");
		return 0;
	}
	// cout << "Done.\n";

	// �ɹ�����
	cout << "Endpoint[" << playerID << "]: Endpoint is running at " << port << "\n";

	running = true; // enable Endpoint::process()
	//running=true��������process()����

	return port;
}//�ɹ������������ض˿ں�


///////////////////////////////////////////////////////////////////// 		
// function��		
//		process()
// Description:                                                       		
// process()�����������̣߳��������̣߳�timer��listenfunc()		
/////////////////////////////////////////////////////////////////////
void Endpoint::process()
{
	while (running)
	{
		online = false;
		timing = true;

		//hub.threadPool.enqueue(&Endpoint::timer, this);
		//hub.threadPool.enqueue(&Endpoint::listenFunc, this);
		int num_threads = std::thread::hardware_concurrency();
		//std::cout << "number of threads = " << num_threads << std::endl;

		//here we should send our functions
		//hub.threadPool.push(std::bind(&Endpoint::timer, this));
		//hub.threadPool.push(std::bind(&Endpoint::listenFunc, this));
		//hub.threadPool.done();
		//for (unsigned int i = 0; i < hub.thread_pool.size(); i++)
		//{
		//	hub.thread_pool.at(i).join();
		//}
		//hub.threadPool.appendTask(std::bind(&Endpoint::timer, this));
		//hub.threadPool.appendTask(std::bind(&Endpoint::listenFunc, this));
		thread timerThread(&Endpoint::timer, this);
		thread listenThread(&Endpoint::listenFunc, this);
		timerThread.join();
		listenThread.join();
	}
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		listenFunc()
// Description:                                                       		
//		�����û�����Ϣ��
/////////////////////////////////////////////////////////////////////
void Endpoint::listenFunc()
{
	// link
	sockaddr_in clientAddr; // client address
	int clientAddrLength = sizeof(clientAddr);
	connSocket = accept(endpointSocket, (sockaddr*)&clientAddr, &clientAddrLength);//��������accept����ʱ��˵���û��Ѿ����ӵ���Endpoint����ʱӦ���ü�ʱ��timerֹͣ��ʱ
	while (timing)
	{
		// cout << "Try to stop timer.\n";
		unique_lock<mutex> lock(mtx);
		online = true;
		lock.unlock();
		cv.notify_one();	//notify_one()������timer()�е�wait for(),�Ӷ�ʹ��timerֹͣ��ʱ
		lock.lock();
	}
	if (connSocket == INVALID_SOCKET)//��������ʧ��
	{
		// TODO
		return;
	}

	//���ӳɹ���
	int ret = recv(connSocket, buf, BUF_LENGTH, 0);//���ܵ�����Ϣ
	/**
	 * recv(connSocket, buf, BUF_LENGTH, 0)
	 * - return bytes of buf
	 * - return 0 if client socket closed or get an empty line
	 * - return SOCKET_ERROR(-1) if server socket is closed or client unexpectedly terminated
	*/
	while (ret != 0 && ret != SOCKET_ERROR && running)
	{
		//���ݿո���ȡ���յ����ַ�
		auto strs = split(buf);
		if (strs[0] == "logout")
		{
			running = false;
		}
		else if (strs[0] == "getPlayerList")
		{
			getPlayerList();
		}
		else if (strs[0] == "resetPassword" && strs.size() == 3)
		{
			resetPassword(strs[1], strs[2]);
		}
		//�е��ɻ�
		else if (strs[0] == "getPokemonList" && strs.size() < 3)
		{
			if (strs.size() == 2)
			{
				getPokemonList(stoi(strs[1]));
			}
			else
			{
				getPokemonList(playerID);
			}
		}
		//
		else if (strs[0] == "getPokemon" && strs.size() == 2)
		{
			getPokemonByID(stoi(strs[1]));
		}
		else if (strs[0] == "pokemonChangeName" && strs.size() == 3)
		{
			pokemonChangeName(strs[1], strs[2]);
		}
		else if (strs[0] == "getDuelStatistic")
		{
			getDuelStatistic();
		}
		else if (strs[0] == "battle" && strs.size() == 5)
		{
			if (strs[1] == "DUEL")
				isDuel = true;
			else
				isDuel = false;
			battle(strs[2], stoi(strs[3]), stoi(strs[4]));
		}
		else if (strs[0] == "chooseBet" && strs.size() == 1)
		{
			chooseBet();
		}
		else if (strs[0] == "discard" && strs.size() == 2)
		{
			discard(strs[1]);
		}
		else
		{
			cout << "Endpoint[" << playerID << "]: Invalid request.\n";
			strcpy(buf, "Reject: Invalid request.\n");
			// send(connSocket, buf, BUF_LENGTH, 0);
		}
		if (running)	//��һ�ֵĽ�����ɣ���ʼ��һ�ֵĽ���
			ret = recv(connSocket, buf, BUF_LENGTH, 0);
	}


	if (!running); //��running=falseʱ����˵����endpoint�Ѿ���ɾ����
	else if (ret == SOCKET_ERROR || ret == 0)	//return 0 if client socket closed or get an empty line
												//return SOCKET_ERROR(-1) if server socket is closed or client unexpectedly terminated
	{
		cout << "Endpoint[" << playerID << "]: Client unexpected offline, start timing.\n";
	}
	else
	{
		// running == false, user logout
		cout << "Endpoint[" << playerID << "]: User logout.\n";
	}
	closesocket(connSocket);
}
///////////////////////////////////////////////////////////////////// 		
// function��		
//		timer()
// Description:                                                       		
//		��ֹ�û��쳣���ߣ��������û����޶�ʱ������������
//		��ʱ��ر�socket������running��Ϊfalse��ֹͣprocess()������ѭ��
/////////////////////////////////////////////////////////////////////
void Endpoint::timer()
{
	using namespace std::chrono_literals;

	// cout << "Start timing.\n";

	/**
	 * wait for player re-login for 10 minutes
	 *
	 * condition_variable::wait_for(lock, time, condition);
	 * - lock is for variables in this function
	 *   - now lock is for bool running
	 * - return false when time out and condition == false
	 * - return true when otherwise
	*/

	//�ȴ����10min��ʱ�����µ�¼����ʱ��ر�socket������running��Ϊfalse��ֹͣprocess()������ѭ�����Ӷ�ʹ��Hub::mornitor������endpoint�ݻ�
	unique_lock<mutex> lock(mtx);
	if (!cv.wait_for(lock, 10min, [this] { return online; }))
	{
		// player is offline
		running = false;
		timing = false;
		closesocket(endpointSocket);
		// cout << "Time up.\n";
	}
	else//����listenfunc�������õ�notify_one()����ʱ��ֹͣ��ʱ
	{
		timing = false;
		// cout << "Stop timing.\n";
	}
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		resetPassword()
// Args:
//		const string& oldPassword
//		const string& newPassword
// Description:                                                       		
//		�������ݿ��в���ָ�����û���,����������
/////////////////////////////////////////////////////////////////////
void Endpoint::resetPassword(const string& oldPassword, const string& newPassword)
{
	// ���ɵ�����
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT name FROM User where id=" + to_string(playerID) + " and password='" + oldPassword + "';";
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		// strcpy(buf, "Reject: Server error.\n");
		strcpy(buf, "����������");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	else if (nRow == 0)
	{
		// wrong password
		sqlite3_free_table(sqlResult);
		// strcpy(buf, "Reject: wrong old password.\n");
		strcpy(buf, "�����벻��ȷ");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	else
	{
		sqlite3_free_table(sqlResult);
	}

	//��������
	sql = "update User set password='" + newPassword + "' where id=" + to_string(playerID) + ";";
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		// strcpy(buf, "Reject: Server error.\n");
		strcpy(buf, "����������");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	else
	{
		sqlite3_free_table(sqlResult);
		strcpy(buf, "Accept.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
	}
	return;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		getPlayerList()
// Description:                                                       		
//		��ȡ����б�
/////////////////////////////////////////////////////////////////////
void Endpoint::getPlayerList()
{
	strcpy(buf, hub.getAllUser().c_str());
	send(connSocket, buf, BUF_LENGTH, 0);
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		getPokemonList()
// Args:
//		playerID
// Description:                                                       		
//		��ȡ�����б�
/////////////////////////////////////////////////////////////////////
void Endpoint::getPokemonList(int playerID)
{
	// get all pokemon from database
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT id, name, race, lv FROM Pokemon where userid=" + to_string(playerID) + ";";
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}


	if (nRow < 3 && playerID == this->playerID)//��С��3������Ϣ
	{
		//���û�����pokemonsֱ������pokemons������Ϊ3��
		for (int i = 0; i < 3 - nRow; ++i)
		{
			int raceIndex = rand() % 4;
			Pokemon t(raceIndex);
			savePokemonToDB(t);	//�浽���ݿ���
		}
		getPokemonList(playerID);//�ٻ�ȡplayerID
	}
	else
	{
		string result;
		//sqlResult[0-3]�ֱ��ʾ����id, name, race, lv�ı�ͷ�����Ե�һ�����ݴ�sqlResult[4]��ʼ
		for (int i = 0; i < nRow; ++i)
		{
			result += sqlResult[4 * (i + 1)]; // id
			result += ' ';
			result += sqlResult[4 * (i + 1) + 1]; // name
			result += ' ';
			result += Pokemon::races[stoi(sqlResult[4 * (i + 1) + 2])]->raceName(); // race
			result += ' ';
			result += sqlResult[4 * (i + 1) + 3]; // lv
			result += '\n';
		}
		strcpy(buf, result.c_str());
		send(connSocket, buf, BUF_LENGTH, 0);
	}
	sqlite3_free_table(sqlResult);
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		getPokemonList()
// Args:
//		const Pokemon&, int 
// Description:                                                       		
//		������С���鷵�ظ�DB
/////////////////////////////////////////////////////////////////////
void Endpoint::savePokemonToDB(const Pokemon& p, int id)
{
	if (id == -1) //��ʾpokemon�����ڣ�������ӵ����ݿ���
	{
		string sql = "INSERT INTO Pokemon(userid, name, race, atk, def, maxHp, speed, lv, exp) VALUES('";
		sql += to_string(playerID) + "','";
		sql += p.name() + "',";
		sql += to_string(p.raceIndex()) + ",";
		sql += to_string(p.atk()) + ",";
		sql += to_string(p.def()) + ",";
		sql += to_string(p.maxHp()) + ",";
		sql += to_string(p.speed()) + ",";
		sql += to_string(p.lv()) + ",";
		sql += to_string(p.exp()) + ");";
		char* errMsg;
		if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
		{
			cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
			sqlite3_free(errMsg);
		}
	}
	else // pokemon���Ѿ����ڣ��������
	{
		string sql = "update Pokemon set atk=";
		sql += to_string(p.atk()) + ", def=";
		sql += to_string(p.def()) + ", maxHp=";
		sql += to_string(p.maxHp()) + ", speed=";
		sql += to_string(p.speed()) + ", lv=";
		sql += to_string(p.lv()) + ", exp=";
		sql += to_string(p.exp());
		sql += " where id=";
		sql += to_string(id) + ";";
		char* errMsg;
		if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
		{
			cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
			sqlite3_free(errMsg);
		}
	}
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		getPokemonByID()
// Args:
//		int pokemonID
// Description:                                                       		
//		ͨ��pokemon��ID, ��ȡС�����ȫ����Ϣ
/////////////////////////////////////////////////////////////////////
void Endpoint::getPokemonByID(int pokemonID)
{
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT id, name, race, atk, def, maxHp, speed, lv, exp FROM Pokemon where id=" + to_string(pokemonID) + ";";	//9������
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	string result;
	result += sqlResult[9 + 0]; // id
	result += ' ';
	result += sqlResult[9 + 1]; // name
	result += ' ';
	result += Pokemon::races[stoi(sqlResult[9 + 2])]->raceName(); // race
	result += ' ';
	result += sqlResult[9 + 3]; // atk
	result += ' ';
	result += sqlResult[9 + 4]; // def
	result += ' ';
	result += sqlResult[9 + 5]; // maxHp
	result += ' ';
	result += sqlResult[9 + 6]; // speed
	result += ' ';
	result += sqlResult[9 + 7]; // lv
	result += ' ';
	result += sqlResult[9 + 8]; // exp
	result += '\n';
	strcpy(buf, result.c_str());
	send(connSocket, buf, BUF_LENGTH, 0);
	sqlite3_free_table(sqlResult);
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		pokemonChangeName()
// Args:
//		const string& pokemonID
//		const string& newName
// Description:                                                       		
//		ͨ��pokemon��ID, �޸�С������ǳ�
/////////////////////////////////////////////////////////////////////
void Endpoint::pokemonChangeName(const string& pokemonID, const string& newName)
{
	string sql = "update Pokemon set name = '" + newName + "' where id=" + pokemonID + ";";

	char* errMsg;
	if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, errMsg);
		send(connSocket, buf, BUF_LENGTH, 0);
	}
	else
	{
		strcpy(buf, "Accept.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
	}
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		getDuelStatistic()
// Description:                                                       		
//		����ս��ͳ����Ϣ��������ʤ�������ܾ�����
/////////////////////////////////////////////////////////////////////
void Endpoint::getDuelStatistic()//����ս��ͳ����Ϣ��������ʤ�������ܾ�����
{
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT win, total FROM User where id=" + to_string(playerID) + ";";
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	string result = sqlResult[2];
	result += ' ';
	result += sqlResult[3];
	strcpy(buf, result.c_str());
	send(connSocket, buf, BUF_LENGTH, 0);
	sqlite3_free_table(sqlResult);
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		battle()
// Args:
//		const string& pokemonID
//		int enemyRaceID
//		int enemyLV
// Description:                                                       		
//		ѡ����ս˫����С����,Ȼ�����˫������ս���ĺ���,������ս�����
/////////////////////////////////////////////////////////////////////
void Endpoint::battle(const string& pokemonID, int enemyRaceID, int enemyLV)
{
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT id, name, race, atk, def, maxHp, speed, lv, exp FROM Pokemon where id=" + pokemonID + ";";//9����
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}
	// construct player pokemon
	Pokemon p1 = Pokemon(sqlResult[10], stoi(sqlResult[11]), stoi(sqlResult[12]), stoi(sqlResult[13]), stoi(sqlResult[14]), stoi(sqlResult[15]), stoi(sqlResult[16]), stoi(sqlResult[17]));

	int currentPokemonID = stoi(sqlResult[9]);

	string result = "";
	result += p1.raceName() + ' ';
	result += to_string(p1.maxHp()) + ' ';
	for (int i = 0; i < 4; ++i)
	{
		result += p1.skillName(i) + ' ';
//		result += p1.skillDscp(i) + ' ';����4������
	}
	result += to_string(p1.lv()) + '\n';

	// construct enemy pokemon
	Pokemon* p2 = Pokemon::getEnemy(enemyRaceID, enemyLV);

	result += p2->raceName() + ' ';
	result += to_string(p2->maxHp()) + ' ';

	strcpy(buf, result.c_str());
	send(connSocket, buf, BUF_LENGTH, 0);

	// construct battle controller
	BattleController battle = BattleController(p1, *p2, connSocket);

	char** sqlResult2;
	int nRow2;
	int nColumn2;
	char* errMsg2;
	string sql2 = "select win, total from User where id=" + to_string(playerID) + ";";
	if (sqlite3_get_table(db, sql2.c_str(), &sqlResult2, &nRow2, &nColumn2, &errMsg2) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg2 << endl;
		sqlite3_free(errMsg2);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}

	if (battle.start())
	{
		// win
		if (isDuel)
		{
			// change win rate
			string sql3 = "update User set win=" + to_string(stoi(sqlResult2[2]) + 1) + ", total=" + to_string(stoi(sqlResult2[3]) + 1) + " where id=" + to_string(playerID) + ";";
			char* errMsg3;
			if (sqlite3_exec(db, sql3.c_str(), nonUseCallback, NULL, &errMsg3) != SQLITE_OK)
			{
				cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg3 << endl;
				sqlite3_free(errMsg3);
			}

			// get a new pokemon
			savePokemonToDB(*p2);
		}

		// p1 gain exp
		if (p1.lv() > p2->lv() + 5)//���p1�ĵȼ���p2�߳���5�������޷���þ���
		{
			// no exp
		}
		else if (p1.lv() >= p2->lv())
		{
			p1.gainExp((p2->lv() - p1.lv() + 5) + 3 + f(2));
		}
		else
		{
			p1.gainExp((p2->lv() - p1.lv()) * 5 + f(5));
		}
	}
	else
	{
		//lose
		// change win rate
		string sql3 = "update User set total=" + to_string(stoi(sqlResult2[3]) + 1) + " where id=" + to_string(playerID) + ";";
		char* errMsg3;
		if (sqlite3_exec(db, sql3.c_str(), nonUseCallback, NULL, &errMsg3) != SQLITE_OK)
		{
			cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg3 << endl;
			sqlite3_free(errMsg3);
		}
	}
	sqlite3_free_table(sqlResult2);

	savePokemonToDB(p1, currentPokemonID);
	delete p2;

	sqlite3_free_table(sqlResult);
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		chooseBet()
// Description:                                                       		
//		���������ѡ3ֻ���鷵�ظ���ң�Ȼ�������ѡһֻ����
/////////////////////////////////////////////////////////////////////
void Endpoint::chooseBet()//
{
	// get all pokemon id
	char** sqlResult;
	int nRow;
	int nColumn;
	char* errMsg;
	string sql;
	sql = "SELECT id FROM Pokemon where userid=" + to_string(playerID) + ";";//��ȡ���о����id
	if (sqlite3_get_table(db, sql.c_str(), &sqlResult, &nRow, &nColumn, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
		strcpy(buf, "Reject: Server error.\n");
		send(connSocket, buf, BUF_LENGTH, 0);
		return;
	}

	vector<string> ids;
	for (int i = 0; i < nRow; ++i)//�����о����id���ַ���װ��vector����ids��
	{
		ids.push_back(sqlResult[i + 1]);
	}

	//�����ȡ3��id��
	int id[3];
	int index[3];
	for (int i = 0; i < 3; ++i)
	{
		index[i] = rand() % ids.size();
		id[i] = stoi(ids[index[i]]);
		ids.erase(ids.begin() + index[i]);//��ids��ɾȥ��Ӧ��id��
	}

	sqlite3_free_table(sqlResult);

	// get 3 pokemon details
	for (int i = 0; i < 3; ++i)
	{
		getPokemonByID(id[i]);
		recv(connSocket, buf, BUF_LENGTH, 0); // read done
	}
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		discard()
// Description:                                                       		
//		��ս�������ʧ�ܣ���ɾȥ����ѡ���id�ŵľ���
/////////////////////////////////////////////////////////////////////
void Endpoint::discard(const string& pokemonID)//ɾȥ����ѡ���id�ŵľ���
{
	string sql = "delete from Pokemon where id=";
	sql += pokemonID + ";";
	char* errMsg;
	if (sqlite3_exec(db, sql.c_str(), nonUseCallback, NULL, &errMsg) != SQLITE_OK)
	{
		cout << "Endpoint[" << playerID << "]: Sqlite3 error: " << errMsg << endl;
		sqlite3_free(errMsg);
	}
}
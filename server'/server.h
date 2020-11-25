#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>
#include "netconfig.h"

// about socket
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

// about database
#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

// about endpoint
#include "endpoint.h"

using namespace std;


///////////////////////////////////////////////////////////////////// 		
// Class:	
//		ThreadPool                            	
// Description:                                                       		
//		线程池
////////////////////////////////////////////////////////////////////
class Function_pool
{

private:
	std::queue<std::function<void()>> m_function_queue;
	std::mutex m_lock;
	std::condition_variable m_data_condition;
	std::atomic<bool> m_accept_functions;

public:

	Function_pool();
	~Function_pool();
	void push(std::function<void()> func);
	void done();
	void infinite_loop_func();
};


//class ThreadPool
//{
//public:
//	using Task = std::function<void()>;
//
//	ThreadPool() : _thread_num(20) {}
//
//	explicit ThreadPool(int num) : _thread_num(num), _is_running(false)
//	{}
//
//	~ThreadPool()
//	{
//		if (_is_running)
//			stop();
//	}
//
//	void start()
//	{
//		_is_running = true;
//
//		// start threads
//		for (int i = 0; i < _thread_num; i++)
//			_threads.emplace_back(std::thread(&ThreadPool::work, this));
//	}
//
//	void stop()
//	{
//		{
//			// stop thread pool, should notify all threads to wake
//			std::unique_lock<std::mutex> lk(_mtx);
//			_is_running = false;
//			_cond.notify_all(); // must do this to avoid thread block
//		}
//
//		// terminate every thread job
//		for (std::thread& t : _threads)
//		{
//			if (t.joinable())
//				t.join();
//		}
//	}
//
//	void appendTask(const Task& task)
//	{
//		if (_is_running)
//		{
//			std::unique_lock<std::mutex> lk(_mtx);
//			_tasks.push(task);
//			_cond.notify_one(); // wake a thread to to the task
//		}
//	}
//
//private:
//	void work()
//	{
//		printf("begin work thread: %d\n", std::this_thread::get_id());
//
//		// every thread will compete to pick up task from the queue to do the task
//		while (_is_running)
//		{
//			Task task;
//			{
//				std::unique_lock<std::mutex> lk(_mtx);
//				if (!_tasks.empty())
//				{
//					// if tasks not empty, 
//					// must finish the task whether thread pool is running or not
//					task = _tasks.front();
//					_tasks.pop(); // remove the task
//				}
//				else if (_is_running && _tasks.empty())
//					_cond.wait(lk);
//			}
//
//			if (task)
//				task(); // do the task
//		}
//
//		printf("end work thread: %d\n", std::this_thread::get_id());
//	}
//
//public:
//	// disable copy and assign construct
//	ThreadPool(const ThreadPool&) = delete;
//	ThreadPool& operator=(const ThreadPool& other) = delete;
//
//private:
//	std::atomic_bool _is_running; // thread pool manager status
//	std::mutex _mtx;
//	std::condition_variable _cond;
//	int _thread_num;
//	std::vector<std::thread> _threads;
//	std::queue<Task> _tasks;
//};
//class ThreadPool {
//public:
//	ThreadPool(unsigned int num = 20);
//	template<class F, class... Args>
//	auto enqueue(F&& f, Args&&... args)
//		->std::future<typename std::result_of<F(Args...)>::type>;
//	~ThreadPool() {
//		{
//			std::unique_lock<std::mutex> lock(queue_mutex);
//			stop = true;
//			condition.notify_all();
//		}
//		for (std::thread& worker : workers)
//			if (worker.joinable())
//				worker.join();
//	}
//	ThreadPool(const ThreadPool&) = delete;
//	ThreadPool& operator=(const ThreadPool& other) = delete;
//private:
//	// need to keep track of threads so we can join them
//	std::vector< std::thread > workers;
//	// the task queue
//	std::queue< std::function<void()> > tasks;
//
//	// synchronization
//	std::mutex queue_mutex;
//	std::condition_variable condition;
//	std::atomic_bool stop;
//	unsigned int _thread_num;
//};


///////////////////////////////////////////////////////////////////// 		
// Class：		
//		Hub                            	
// Description:                                                       		
//		登录前定义了一些接口函数，负责处理注册和登录的请求
//		登录之后在Endpoint的类中使用这些接口函数                                               		
/////////////////////////////////////////////////////////////////////

class Hub
{
private:
	// about network
	const int HUB_PORT = 7500;	//hub的端口号为7500
	SOCKET hubSocket;
	SOCKET connSocket;
	volatile bool running;
	char buf[BUF_LENGTH];

	//about database
	sqlite3* db;
	
	// about endpoint
	vector<Endpoint*> endpoints;

	// about thread
	mutex mtx; // to protect endpoints

	// 接口函数
	void login(const string& username, const string& password);//登录函数，返回端口号说明登录成功
	void logon(const string& username, const string& password);//注册函数，返回Accepted.\n则说明注册成功
																

	// authentication
	bool isValidUsername(const string& str);
	bool isValidPassword(const string& str);

	// 线程函数
	void listenFunc();//实现正常的阻塞监听
	void terminateFunc();//实现按键停止服务器
	void mornitor(Endpoint* const endpoint);//一个线程函数，每新建一个Endpoint时，调用这个函数来新建一个线程

	// singleton
	Hub() {}; //将hub的构造函数隐藏起来
	Hub(Hub const&) = delete;
	Hub(Hub&&) = delete;
	Hub& operator=(Hub const&) = delete;
	~Hub();

public:
	static Hub& getInstance(); // 将hub设为单件类，只能使用静态public的方式来构造类的实例

	void start(); // init database and socket
	string getAllUser();

	//Thread Pool
	Function_pool threadPool;
	int num_threads = std::thread::hardware_concurrency();
	//std::cout << "number of threads = " << num_threads << std::endl;
	std::vector<std::thread> thread_pool;

};

inline int nonUseCallback(void* notUsed, int argc, char** argv, char** azColName) { return 0; }	//表示没有使用回调函数
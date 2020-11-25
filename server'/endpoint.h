#pragma once

#include "netconfig.h"
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include "pokemon.h"

// about socket
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

// about database
#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

using namespace std;

class Hub;

///////////////////////////////////////////////////////////////////// 		
// Class：		
//		Endpoint                            	
// Description:                                                       		
//		定义了用户接受端点，有一个Hub标记与它相连的Server
//		连接一个数据库，用来操作数据，持久化操作
//		每一个用户端点需要ip地址，socket套接字，标记是否在线的状态，用来放止异常退出
//		一个缓冲区，用来发送和接受来自服务器的信息
//		每一个用户端点需要ID号来标识自己
//		线程需要信号量，锁
//		还有修改密码的方法，获取玩家列表的方法，修改宠物精灵的名称，对战功能的方法
//		返回统计信息，获取初始精灵
/////////////////////////////////////////////////////////////////////
class Endpoint
{

	// about database
	sqlite3*& db;/* An open database */

	// about network
	int port;
	string ip;
	SOCKET endpointSocket;
	SOCKET connSocket;
	volatile bool running;
	volatile bool online;
	char buf[BUF_LENGTH];

	// about player
	int playerID;
	string playerUsername;
	bool isDuel;

	// about thread
	mutex mtx;
	condition_variable cv;
	volatile bool timing; // 用来表示线程函数timer()正在运行

	// thread function
	void timer();
	void listenFunc();

	// 一些接口函数
	void resetPassword(const string& oldPassword, const string& newPassword);//重置密码的函数
	void getPlayerList();//获取玩家表的函数
	void getPokemonList(int playerID);//获取pokemon表
	void getPokemonByID(int pokemonID);//获取pokemonid
	void pokemonChangeName(const string& pokemonID, const string& newName);//pokemon 改名
	void battle(int pokemonID, bool autoFight = false);
	void useSkill(int skillID);


	void getDuelStatistic(); //返回玩家的战斗统计信息，包括获胜场数和总决斗场数
							//格式是win total


	void battle(const string& pokemonID, int enemyRaceID, int enemyLV);


	void chooseBet();//服务器挑选三个精灵返回给玩家，玩家选择一个丢弃
					//调用了getPokemonByID()返回三次信息，每次返回一个pokemon的详情



	void discard(const string& pokemonID);//丢弃指定的id号的pokemon

	// other functions
	void savePokemonToDB(const Pokemon& p, int id = -1);

public:
	// about server
	Hub& hub;

	Endpoint(int playerID, sqlite3*& db, Hub& hub);
	~Endpoint();

	int start();		//start()函数启动服务并返回端口号，如果返回0则表示启动失败

	void process(); //里面写socket的accept函数，并在process函数中处理各种请求，返回时代表endpoint结束（超时或用户退出）

	bool isOnline() const { return online; }
	int getPlayerID() const { return playerID; }
	string getPlayerName() const { return playerUsername; }
	int getPort() const { return port; }
};
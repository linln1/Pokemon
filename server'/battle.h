#pragma once

#include "pokemon.h"
#include "netconfig.h"
#include <string>

// about socket
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

///////////////////////////////////////////////////////////////////// 		
// Class：		
//		BattleController                            	
// Description:                                                       		
//		战斗类，用来计时，让战斗双方轮流进攻
/////////////////////////////////////////////////////////////////////
class BattleController
{
private:
	Pokemon& p1, & p2;
	int timer1;
	int timer2;
	char buf[BUF_LENGTH];
	SOCKET& connSocket;	//建立连接的套接字connSocket
	string msg;

public:
	BattleController(Pokemon& playerPokemon, Pokemon& enemyPokemon, SOCKET& connSocket);
	//构造函数中将socket传给battleController，从而使得该类可以调用recv函数来阻塞战斗流程，给用户选择技能

	bool start(); // return true if playerPokemon won
};
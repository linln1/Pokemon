#pragma once

#include "pokemon.h"
#include "netconfig.h"
#include <string>

// about socket
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

///////////////////////////////////////////////////////////////////// 		
// Class��		
//		BattleController                            	
// Description:                                                       		
//		ս���࣬������ʱ����ս��˫����������
/////////////////////////////////////////////////////////////////////
class BattleController
{
private:
	Pokemon& p1, & p2;
	int timer1;
	int timer2;
	char buf[BUF_LENGTH];
	SOCKET& connSocket;	//�������ӵ��׽���connSocket
	string msg;

public:
	BattleController(Pokemon& playerPokemon, Pokemon& enemyPokemon, SOCKET& connSocket);
	//���캯���н�socket����battleController���Ӷ�ʹ�ø�����Ե���recv����������ս�����̣����û�ѡ����

	bool start(); // return true if playerPokemon won
};
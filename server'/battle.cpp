#define _CRT_SECURE_NO_WARNINGS // to disable sscanf

#include "battle.h"

BattleController::BattleController(Pokemon& playerPokemon, Pokemon& enemyPokemon, SOCKET& connSocket) : p1(playerPokemon), p2(enemyPokemon), connSocket(connSocket)
{
}

///////////////////////////////////////////////////////////////////// 		
// Class：		
//		start
// Description:                                                       		
//		控制战斗过程，战斗过程中的通信过程
//		
/////////////////////////////////////////////////////////////////////
bool BattleController::start()
{
	// dbout << p1.name() << " VS " << p2.name() << "!\n";
	// dbout << "Battle Start!\n\n";
	// 战斗开始前先将双方的状态值全部恢复
	p1.restoreAll();
	p2.restoreAll();

	timer1 = 0;
	timer2 = 0;
	//根据两个小精灵的速度属性，来判断攻击频率，双方进行进攻
	while (1)
	{
		if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR) 
			return false; // get done
		while (timer1 < p1.cspeed() && timer2 < p2.cspeed())
		{
			timer1++;
			timer2++;
		}

		int index;
		//手动战斗过程
		if (timer1 >= p1.cspeed() && timer2 >= p2.cspeed())
		{
			if (p1.cspeed() >= p2.cspeed())
			{
				send(connSocket, "turn", BUF_LENGTH, 0);//发送信息
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR) 
					return false; // get player pokemon skill index
				sscanf(buf, "%d", &index);//获取玩家pokemon的skill index，sccanf()用于取buffer中第一个整型字符并付给index
				msg = "1 ";
				if (p1.attack(p2, index, msg)) // 手动战斗
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);//发送buf，内容为msg
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR)
					return false; // get done
				msg = "0 ";
				if (p2.attack(p1, msg)) //对手自动战斗
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);//发送buf，内容为msg
			}
			else
			{
				msg = "0 ";
				if (p2.attack(p1, msg))
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR)
					return false; // get done
				send(connSocket, "turn", BUF_LENGTH, 0);
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR) 
					return false;
				sscanf(buf, "%d", &index);
				msg = "1 ";
				if (p1.attack(p2, index, msg))
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);
			}
			timer1 = timer2 = 0;
		}
		else if (timer1 >= p1.cspeed())
		{
			//p2 attack
			msg = "0 ";
			if (p2.attack(p1, msg))
				break;
			strcpy(buf, msg.c_str());
			send(connSocket, buf, BUF_LENGTH, 0);
			timer1 = 0;
		}
		else
		{
			send(connSocket, "turn", BUF_LENGTH, 0);
			if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR) 
				return false;
			sscanf(buf, "%d", &index);
			msg = "1 ";
			if (p1.attack(p2, index, msg))
				break;
			strcpy(buf, msg.c_str());
			send(connSocket, buf, BUF_LENGTH, 0);
			timer2 = 0;
		}
	}

	strcpy(buf, msg.c_str());
	send(connSocket, buf, BUF_LENGTH, 0);
	//判断最后HP谁比较高，高的赢
	if (p1.hp())//p1赢
	{
		// dbout << p1.name() << " won!\n\n";
		return true;
	}
	else//p1输
	{
		// dbout << p2.name() << " won!\n\n";
		return false;
	}
}

#define _CRT_SECURE_NO_WARNINGS // to disable sscanf

#include "battle.h"

BattleController::BattleController(Pokemon& playerPokemon, Pokemon& enemyPokemon, SOCKET& connSocket) : p1(playerPokemon), p2(enemyPokemon), connSocket(connSocket)
{
}

///////////////////////////////////////////////////////////////////// 		
// Class��		
//		start
// Description:                                                       		
//		����ս�����̣�ս�������е�ͨ�Ź���
//		
/////////////////////////////////////////////////////////////////////
bool BattleController::start()
{
	// dbout << p1.name() << " VS " << p2.name() << "!\n";
	// dbout << "Battle Start!\n\n";
	// ս����ʼǰ�Ƚ�˫����״ֵ̬ȫ���ָ�
	p1.restoreAll();
	p2.restoreAll();

	timer1 = 0;
	timer2 = 0;
	//��������С������ٶ����ԣ����жϹ���Ƶ�ʣ�˫�����н���
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
		//�ֶ�ս������
		if (timer1 >= p1.cspeed() && timer2 >= p2.cspeed())
		{
			if (p1.cspeed() >= p2.cspeed())
			{
				send(connSocket, "turn", BUF_LENGTH, 0);//������Ϣ
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR) 
					return false; // get player pokemon skill index
				sscanf(buf, "%d", &index);//��ȡ���pokemon��skill index��sccanf()����ȡbuffer�е�һ�������ַ�������index
				msg = "1 ";
				if (p1.attack(p2, index, msg)) // �ֶ�ս��
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);//����buf������Ϊmsg
				if (recv(connSocket, buf, BUF_LENGTH, 0) == SOCKET_ERROR)
					return false; // get done
				msg = "0 ";
				if (p2.attack(p1, msg)) //�����Զ�ս��
					break;
				strcpy(buf, msg.c_str());
				send(connSocket, buf, BUF_LENGTH, 0);//����buf������Ϊmsg
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
	//�ж����HP˭�Ƚϸߣ��ߵ�Ӯ
	if (p1.hp())//p1Ӯ
	{
		// dbout << p1.name() << " won!\n\n";
		return true;
	}
	else//p1��
	{
		// dbout << p2.name() << " won!\n\n";
		return false;
	}
}

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
// Class��		
//		Endpoint                            	
// Description:                                                       		
//		�������û����ܶ˵㣬��һ��Hub�������������Server
//		����һ�����ݿ⣬�����������ݣ��־û�����
//		ÿһ���û��˵���Ҫip��ַ��socket�׽��֣�����Ƿ����ߵ�״̬��������ֹ�쳣�˳�
//		һ�����������������ͺͽ������Է���������Ϣ
//		ÿһ���û��˵���ҪID������ʶ�Լ�
//		�߳���Ҫ�ź�������
//		�����޸�����ķ�������ȡ����б�ķ������޸ĳ��ﾫ������ƣ���ս���ܵķ���
//		����ͳ����Ϣ����ȡ��ʼ����
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
	volatile bool timing; // ������ʾ�̺߳���timer()��������

	// thread function
	void timer();
	void listenFunc();

	// һЩ�ӿں���
	void resetPassword(const string& oldPassword, const string& newPassword);//��������ĺ���
	void getPlayerList();//��ȡ��ұ�ĺ���
	void getPokemonList(int playerID);//��ȡpokemon��
	void getPokemonByID(int pokemonID);//��ȡpokemonid
	void pokemonChangeName(const string& pokemonID, const string& newName);//pokemon ����
	void battle(int pokemonID, bool autoFight = false);
	void useSkill(int skillID);


	void getDuelStatistic(); //������ҵ�ս��ͳ����Ϣ��������ʤ�������ܾ�������
							//��ʽ��win total


	void battle(const string& pokemonID, int enemyRaceID, int enemyLV);


	void chooseBet();//��������ѡ�������鷵�ظ���ң����ѡ��һ������
					//������getPokemonByID()����������Ϣ��ÿ�η���һ��pokemon������



	void discard(const string& pokemonID);//����ָ����id�ŵ�pokemon

	// other functions
	void savePokemonToDB(const Pokemon& p, int id = -1);

public:
	// about server
	Hub& hub;

	Endpoint(int playerID, sqlite3*& db, Hub& hub);
	~Endpoint();

	int start();		//start()�����������񲢷��ض˿ںţ��������0���ʾ����ʧ��

	void process(); //����дsocket��accept����������process�����д���������󣬷���ʱ����endpoint��������ʱ���û��˳���

	bool isOnline() const { return online; }
	int getPlayerID() const { return playerID; }
	string getPlayerName() const { return playerUsername; }
	int getPort() const { return port; }
};
#pragma once

#include <string>
#include <stdlib.h>
#include <iostream>

using namespace std;

class Pokemon; //����

///////////////////////////////////////////////////////////////////// 		
// Enum��		
//		PokemonType                      	
// Description:                                                       		
//		�������ͣ���ATk����HP����DEF����SPE��������
/////////////////////////////////////////////////////////////////////
enum PokemonType
{
	ATK, //high attack
	HP,	//high HP
	DEF, //high defence
	SPE	//high speed
};

///////////////////////////////////////////////////////////////////// 		
// Class��		
//		PokemonBase                            	
// Description:                                                       		
//		������С�����������ԣ���������������������������������Ѫ���������ٶȣ��������ߣ�����
//		������С�����һЩ��������ȡС�������������������������������������������������Ѫ���������ٶȣ��������Ƶ�
/////////////////////////////////////////////////////////////////////
class PokemonBase
{
protected: //all those attributes can be changed by derived classes
	string _raceName;
	PokemonType _type;
	int _baseAtk; //�ܵ��������͵�Ӱ�죬��߹�������Ѫ��
	int _baseDef;
	int _baseHp;
	int _baseSpeed;
	int _expCurve[14];		//LV2- LV15
	string _skillName[4]; //���ܣ�4�� 1�չ���3���⹥��
	
public:
	PokemonBase(PokemonType type);//���캯��������ΪС�������������
	virtual ~PokemonBase() = default;

	//getter
	string raceName() const { return _raceName; }//��ȡ����������
	PokemonType type() const { return _type; }
	int baseAtk() const { return _baseAtk; }
	int baseDef() const { return _baseDef; }
	int baseHp() const { return _baseHp; }
	int baseSpeed() const { return _baseSpeed; }
	string skillName(int n) const;


	//about level-up
	int expCurve(int level) const; // level����[2,15]

	//����Ϊ�麯��������������Լ����ͨ��ָ�����
	//������������սʱ���ڸı�С����ĵ�ǰ״̬��
	virtual bool attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex = 0) const = 0; //yes, this is a CONST method
	
	//�ж��Ƿ�ɹ�����																								 //�ж��Ƿ�ɹ�����
	bool dodge(int attacker, int aim, string& msg) const;
};


///////////////////////////////////////////////////////////////////// 		
// Class��		
//		PokemonBase                            	
// Description:                                                       		
//		������С��������ԣ�����������������Ѫ�����ٶȣ��������ߣ�����
//		��ʼ��С�����ʵ�������캯������������
//		������С�����һЩ��������ȡС�������������������������������������������������Ѫ���������ٶȣ��������Ƶ�
//		���ض�ս�����е�״̬����������Ҫ�Ĺ��������������������ٶȣ�Ѫ����
/////////////////////////////////////////////////////////////////////
class Pokemon
{
private:
	int _raceIndex;
	string _name;
	int _atk;
	int _def;
	int _maxHp;
	int _speed;
	int _lv; //level
	int _exp;

	//in battle
	int _hp;
	int _catk; //current atk
	int _cdef;
	int _cspeed;
	int _cpp[3]; //power point

public:
	Pokemon(int raceIndex, const string& name = "");//��ʼ��һ���¾���																										
	Pokemon(const string& name, int raceIndex, int atk, int def, int maxHp, int speed, int lv, int exp); // ctor for database
	~Pokemon();

	static const PokemonBase* races[4];//��̬�ĳ�Ա����races��ΪPokemonBase��ָ��
										//�Ծ�̬�ķ�ʽ��ʼ����������

	static Pokemon* getEnemy(int raceIndex, int lv);//��̬����getEnemy���ܹ�ͨ����������͵ȼ�����һ��������Ϊ��Ҿ���Ķ��֡�

	//��ȡƽʱ״̬��
	string name() const { return _name; }
	int atk() const { return _atk; }
	int def() const { return _def; }
	int maxHp() const { return _maxHp; }
	int speed() const { return _speed; }
	int lv() const { return _lv; }
	int exp() const { return _exp; }
	//��ȡʵʱ״̬��
	string raceName() const { return races[_raceIndex]->raceName(); }
	string raceType() const;
	int raceIndex() const { return _raceIndex; }
	string skillName(int n) const { return races[_raceIndex]->skillName(n); }
	//��սʱ���洢
	void restoreAll();
	bool attack(Pokemon& aim, string& msg);	//�Զ�ս��
	bool attack(Pokemon& aim, int skillIndex, string& msg); // �ֶ�ս��
	bool takeDamage(int n);
	bool gainExp(int count); //�������Ƿ���ֵ��true
	//getter
	int catk() const { return _catk; }
	int cdef() const { return _cdef; }
	int hp() const { return _hp; }
	int cspeed() const { return _cspeed; }

	//���ù��������������������ٶȣ�Ѫ��
	void changeAtk(int count);
	void changeDef(int count);
	void changeSpeed(int count);
	bool changeHp(int count); //��Ѫ��Ϊ0ʱ����ֵΪtrue

	Pokemon& operator=(const Pokemon&) = delete;//ɾ��Ĭ�����������
};

int f(int n); //���һ����-n��n�������


///////////////////////////////////////////////////////////////////// 		
// Class��		
//		Race �̳�PokemonBase                            	
// Description:                                                       		
//		����С���������
/////////////////////////////////////////////////////////////////////
template <int N>
class Race : public PokemonBase
{
	public:
		Race();
		virtual ~Race() = default;
		bool attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex = 0) const;
};
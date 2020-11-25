#pragma once

#include <string>
#include <stdlib.h>
#include <iostream>

using namespace std;

class Pokemon; //声明

///////////////////////////////////////////////////////////////////// 		
// Enum：		
//		PokemonType                      	
// Description:                                                       		
//		四种类型，高ATk，高HP，高DEF，高SPE四种类型
/////////////////////////////////////////////////////////////////////
enum PokemonType
{
	ATK, //high attack
	HP,	//high HP
	DEF, //high defence
	SPE	//high speed
};

///////////////////////////////////////////////////////////////////// 		
// Class：		
//		PokemonBase                            	
// Description:                                                       		
//		定义了小精灵基类的属性，包括基本攻击力，基本防御力，基本血量，基本速度，经验曲线，技能
//		定义了小精灵的一些方法，获取小精灵的种族名，类型名，基本攻击力，基本防御力，基本血量，基本速度，技能名称等
/////////////////////////////////////////////////////////////////////
class PokemonBase
{
protected: //all those attributes can be changed by derived classes
	string _raceName;
	PokemonType _type;
	int _baseAtk; //受到种族类型的影响，如高攻击、高血量
	int _baseDef;
	int _baseHp;
	int _baseSpeed;
	int _expCurve[14];		//LV2- LV15
	string _skillName[4]; //技能，4个 1普攻，3特殊攻击
	
public:
	PokemonBase(PokemonType type);//构造函数，参数为小精灵的种族类型
	virtual ~PokemonBase() = default;

	//getter
	string raceName() const { return _raceName; }//获取种族类型名
	PokemonType type() const { return _type; }
	int baseAtk() const { return _baseAtk; }
	int baseDef() const { return _baseDef; }
	int baseHp() const { return _baseHp; }
	int baseSpeed() const { return _baseSpeed; }
	string skillName(int n) const;


	//about level-up
	int expCurve(int level) const; // level属于[2,15]

	//设置为虚函数方便子类访问以及外界通过指针访问
	//攻击函数，对战时便于改变小精灵的当前状态量
	virtual bool attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex = 0) const = 0; //yes, this is a CONST method
	
	//判断是否成功闪避																								 //判断是否成功闪避
	bool dodge(int attacker, int aim, string& msg) const;
};


///////////////////////////////////////////////////////////////////// 		
// Class：		
//		PokemonBase                            	
// Description:                                                       		
//		定义了小精灵的属性，攻击力，防御力，血量，速度，经验曲线，技能
//		初始化小精灵的实例，构造函数，析构函数
//		定义了小精灵的一些方法，获取小精灵的种族名，类型名，基本攻击力，基本防御力，基本血量，基本速度，技能名称等
//		返回对战过程中的状态，升级所需要的攻击能力，防御能力，速度，血量等
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
	Pokemon(int raceIndex, const string& name = "");//初始化一个新精灵																										
	Pokemon(const string& name, int raceIndex, int atk, int def, int maxHp, int speed, int lv, int exp); // ctor for database
	~Pokemon();

	static const PokemonBase* races[4];//静态的成员变量races作为PokemonBase的指针
										//以静态的方式初始化四种种族

	static Pokemon* getEnemy(int raceIndex, int lv);//静态函数getEnemy，能够通过敌人种族和等级构造一个精灵作为玩家精灵的对手。

	//获取平时状态量
	string name() const { return _name; }
	int atk() const { return _atk; }
	int def() const { return _def; }
	int maxHp() const { return _maxHp; }
	int speed() const { return _speed; }
	int lv() const { return _lv; }
	int exp() const { return _exp; }
	//获取实时状态量
	string raceName() const { return races[_raceIndex]->raceName(); }
	string raceType() const;
	int raceIndex() const { return _raceIndex; }
	string skillName(int n) const { return races[_raceIndex]->skillName(n); }
	//对战时，存储
	void restoreAll();
	bool attack(Pokemon& aim, string& msg);	//自动战斗
	bool attack(Pokemon& aim, int skillIndex, string& msg); // 手动战斗
	bool takeDamage(int n);
	bool gainExp(int count); //当升级是返回值是true
	//getter
	int catk() const { return _catk; }
	int cdef() const { return _cdef; }
	int hp() const { return _hp; }
	int cspeed() const { return _cspeed; }

	//设置攻击能力，防御能力，速度，血量
	void changeAtk(int count);
	void changeDef(int count);
	void changeSpeed(int count);
	bool changeHp(int count); //当血量为0时返回值为true

	Pokemon& operator=(const Pokemon&) = delete;//删除默认运算符重载
};

int f(int n); //获得一个从-n到n的随机数


///////////////////////////////////////////////////////////////////// 		
// Class：		
//		Race 继承PokemonBase                            	
// Description:                                                       		
//		描述小精灵的种族
/////////////////////////////////////////////////////////////////////
template <int N>
class Race : public PokemonBase
{
	public:
		Race();
		virtual ~Race() = default;
		bool attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex = 0) const;
};
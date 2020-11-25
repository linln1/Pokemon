/*****************************************************************
 * Copyright (c)2020, by gpf/lzy
 * All rights reserved.
 * FileName��		pokemon.cpp
 * System��          	Windows
 * SubSystem��        	Common System Service
 * Author��		gpf/lzy
 * Date��		2020.10.31
 * Version��		1.0
 * Description��
		��Pokemon��ľ���ʵ��
 *
 * Last Modified:

	2020.11.24, 	By lzy
*/


#include "pokemon.h"

const PokemonBase* Pokemon::races[4] = { new Race<0>(), new Race<1>(), new Race<2>(), new Race<3>() };

///////////////////////////////////////////////////////////////////// 		
// function��		
//		getEnemy()
// Args:
//		int raceIndex
//		int lv
// Description:                                                       		
//		generate a pokemon with raceIndex and lv
/////////////////////////////////////////////////////////////////////
Pokemon* Pokemon::getEnemy(int raceIndex, int lv)
{
	if (raceIndex < 0 || raceIndex > 3)//�������ƣ�����
		return 0;
	if (lv < 1 || lv > 15)//�ȼ�����
		return 0;
	Pokemon* result = new Pokemon(raceIndex);
	while (result->lv() < lv)//���ϵĻ�ȡ����ֵ���Ӷ����ɳ�һ����Ӧ�ȼ��Ķ���
	{
		result->gainExp(10);
	}
	return result;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		PokemonBase()
// Args:
//		PokemonType type
// Description:                                                       		
//		���캯��
/////////////////////////////////////////////////////////////////////
PokemonBase::PokemonBase(PokemonType type)
{
	_type = type;

	//��׼����״̬��������Ѫ��30������10������5���ٶ�10
	_baseAtk = 10;
	_baseDef = 5;
	_baseHp = 30;
	_baseSpeed = 10;

	//type talent
	switch (type)
	{
	case ATK:
		_baseAtk += 3;
		break;
	case DEF:
		_baseDef += 3;
		break;
	case HP:
		_baseHp += 10;
		break;
	case SPE:
		_baseSpeed += 5;
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		skillName()
// Args:
//		int n
// Description:                                                       		
//		���ص�n���ܵ�����
/////////////////////////////////////////////////////////////////////
string PokemonBase::skillName(int n) const
{
	if (n >= 0 && n <= 3)
	{
		return _skillName[n];
	}
	return "";
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		expCurve()
// Args:
//		int level
// Description:                                                       		
//		����������Ҫ�ľ���
/////////////////////////////////////////////////////////////////////
int PokemonBase::expCurve(int level) const
{
	if (level <= 15 && level >= 2)
	{
		return _expCurve[level - 2];
	}
	return 0;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		dodge()
// Args:
//		int level
//		int aim
//		string& args
// Description:                                                       		
//		msg���ظ��ͻ��˵�ս����Ϣ:1��ʾmiss,0��ʾ�ɹ�����
/////////////////////////////////////////////////////////////////////
bool PokemonBase::dodge(int attacker, int aim, string& msg) const
{
	if ((attacker + f(40)) / 80 - aim / 150 < 0)
	{
		// dbout << "Miss!\n";
		msg += "1 ";
		return true;
	}
	msg += "0 ";
	return false;
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		Pokemon()
// Args:
//		int raceIndex
//		const string& name
// Description:                                                       		
//		���캯��
/////////////////////////////////////////////////////////////////////
Pokemon::Pokemon(int raceIndex, const string& name)
{
	_raceIndex = raceIndex;

	if (!name.length())
	{
		_name = races[_raceIndex]->raceName(); //ʹ��racename��ΪĬ������
	}
	else
	{
		_name = name;
	}

	//�����������
	_atk = races[_raceIndex]->baseAtk() + f(3);
	_def = races[_raceIndex]->baseDef() + f(2);
	_maxHp = _hp = races[_raceIndex]->baseHp() + f(5);
	_speed = races[_raceIndex]->baseSpeed() + f(3);

	_lv = 1;
	_exp = 0;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		Pokemon()
// Args:
//		const string& name
//		int raceIndex
//		int atk
//		int def
//		int maxHp
//		int speed
//		int lv
//		int exp
// Description:                                                       		
//		���캯��
/////////////////////////////////////////////////////////////////////
Pokemon::Pokemon(const string& name, int raceIndex, int atk, int def, int maxHp, int speed, int lv, int exp)
{
	_name = name;
	_raceIndex = raceIndex;
	_atk = atk;
	_def = def;
	_maxHp = maxHp;
	_speed = speed;
	_lv = lv;
	_exp = exp;
}

Pokemon::~Pokemon()
{
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		raceType()
// Description:                                                       		
//		����С��������
/////////////////////////////////////////////////////////////////////
string Pokemon::raceType() const
{
	switch (races[_raceIndex]->type())
	{
	case ATK:
		return "High Attack";
	case HP:
		return "High HP";
	case DEF:
		return "High Defence";
	case SPE:
		return "High Speed";
	default:
		break;
	}
	return "";
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		changeAtk()
// Args:
//		int count
// Description:                                                       		
//		�޸Ĺ�����
/////////////////////////////////////////////////////////////////////
void Pokemon::changeAtk(int count)
{
	_catk += count;
	if (_catk < 1)
		_catk = 1;

}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		changeDef()
// Args:
//		int count
// Description:                                                       		
//		�޸ķ�����
/////////////////////////////////////////////////////////////////////
void Pokemon::changeDef(int count)
{
	_cdef += count;
	if (_cdef < 1)
		_cdef = 1;

}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		changeSpeed()
// Args:
//		int count
// Description:                                                       		
//		�޸��ٶ�
/////////////////////////////////////////////////////////////////////
void Pokemon::changeSpeed(int count)
{
	_cspeed += count;
	if (_cspeed < 1)
		_cspeed = 1;

}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		changeHp()
// Args:
//		int count
// Description:                                                       		
//		�޸�Hp
/////////////////////////////////////////////////////////////////////
bool Pokemon::changeHp(int count)
{
	_hp += count;

	if (_hp > _maxHp)
		_hp = _maxHp;
	if (_hp < 0)
		_hp = 0;

	if (!_hp)
	{
		// dbout << _name << " is down!\n\n";
		return true;
	}
	else
	{
		// dbout << _name << "'s HP becomes " << _hp << endl
		// << endl;
	}
	return false;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		restoreAll()
// Description:                                                       		
//		ս����֮�󣬻ظ��������������
/////////////////////////////////////////////////////////////////////
void Pokemon::restoreAll()
{
	_hp = _maxHp;
	_catk = _atk;
	_cdef = _def;
	_cspeed = _speed;

}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		gainExp()
// Args:
//		int count
// Description:                                                       		
//		�޸�Hp
/////////////////////////////////////////////////////////////////////
bool Pokemon::gainExp(int count)
{
	if (_lv == 15)
		return false;

	if (count <= 0)
		count = 1;

	_exp += count;

	// dbout << _name << " gains " << count << " exp!\n";
	// dbout << "Now " << _name << " has " << _exp << " exp\n"
	// << endl;

	bool LV_UP = false;
	while (_lv < 15 && _exp > races[_raceIndex]->expCurve(_lv + 1))
	{
		//level-up!
		LV_UP = true;
		++_lv;
		// dbout << "Level Up!\n";
		// dbout << _name << "'s now LV" << _lv << "!\n"
		// << endl;

		//increase attributes
		int atk, def, maxHp, speed;
		atk = 4 + f(1);
		def = 2 + f(1);
		maxHp = 20 + f(5);
		speed = 5 + f(1);

		//race talent
		switch (races[_raceIndex]->type())
		{
		case ATK:
			atk += 3;
			break;
		case HP:
			maxHp += 15;
			break;
		case DEF:
			def += 3;
			break;
		case SPE:
			speed += 3;
			break;
		default:
			break;
		}

		_atk += atk;
		_def += def;
		_maxHp += maxHp;
		_speed += speed;

		// dbout << "Atk: " << _atk - atk << "->" << _atk << "!\n";
		// dbout << "Def: " << _def - def << "->" << _def << "!\n";
		// dbout << "MaxHP: " << _maxHp - maxHp << "->" << _maxHp << "!\n";
		// dbout << "Speed: " << _speed - speed << "->" << _speed << "!\n\n";
	}

	if (LV_UP)
		return true;

	return false; //default
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		attack()
// Args:
//		Pokemon& aim
//		string& msg
// Description:                                                       		
//		����ս�����ܺ͹���������������ֵ
/////////////////////////////////////////////////////////////////////
bool Pokemon::attack(Pokemon& aim, string& msg)
{
	int skillIndex = 0;//���ܱ��

	bool usable[3];//�жϼ����Ƿ����
	int usableCount = 1;				//���õļ�������
	for (int i = 0; i < 3; ++i) //��ȡ���п��õļ���
	{
		if (_lv >= (i + 1) * 5)
		{
			usable[i] = true;
			++usableCount;
		}
	}
	//�Զ�ս�������ʹ�ü���
	int use = rand() % usableCount;
	//Ѱ��Ҫ�Զ��ͷŵļ���
	if (!use)//use=0ʱ���ͷŵ�һ������
		skillIndex = 0;
	else if (usable[use - 1])
	{
		skillIndex = use;
	}

	return races[_raceIndex]->attack(*this, aim, msg, skillIndex);
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		attack()
// Args:
//		Pokemon& aim
//		int skillIndex
//		string& msg
// Description:                                                       		
//		����ս�����ܺ͹���������������ֵ
/////////////////////////////////////////////////////////////////////
bool Pokemon::attack(Pokemon& aim, int skillIndex, string& msg)
{
	int space = 0; //count space
	for (int i = 0; i * 5 <= _lv; ++i)
	{
		if (skillName(i).length() > space)
			space = skillName(i).length();
	}
	space += 4;
	for (int i = 0; i * 5 <= _lv; ++i)
	{
		// dbout << "	" << i + 1 << ": " << skillName(i);

	}

	if (skillIndex < 0 || skillIndex > 3)
		skillIndex = 0;
	if (skillIndex * 5 <= _lv)
	{
		return races[_raceIndex]->attack(*this, aim, msg, skillIndex);
	}

	return races[_raceIndex]->attack(*this, aim, msg, 0);
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		takeDamage()
// Args:
//		int n
// Description:                                                       		
//		����˺�
/////////////////////////////////////////////////////////////////////
bool Pokemon::takeDamage(int n)
{
	if (n < 10)//Ĭ���˺�Ϊ10������ֵ
		n = 10;
	return changeHp(-n);
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		f()
// Args:
//		int n
// Description:                                                       		
//		��-n��n�ķ�Χ�õ�һ������ֵ
/////////////////////////////////////////////////////////////////////
int f(int n)//��-n��n�ķ�Χ�õ�һ������ֵ
{
	return rand() % (2 * n + 1) - n;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		Race()
// Description:
//		����С���ļ���
/////////////////////////////////////////////////////////////////////
template <>
Race<0>::Race() : PokemonBase(ATK)
{
	// _raceName = "xiaohuohou";
	_raceName = "xiaohuohou";
	_expCurve[0] = 5;
	for (int i = 1; i < 14; ++i)
	{
		_expCurve[i] = _expCurve[i - 1] + 5 * i;
	}
	_skillName[0] = "zhua";
	_skillName[1] = "huohua";
	_skillName[2] = "huoyanche";
	_skillName[3] = "lieyanchongzhuang";
}

template <>
bool Race<0>::attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex) const
{
	// dbout << attacker.name() << " uses " << attacker.skillName(skillIndex) << "!\n";

	msg += attacker.skillName(skillIndex) + ' ';

	switch (skillIndex)
	{
	case 1: //��
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() + attacker.lv() * 2 - aim.cdef() / 2 + f(4);
		
		bool result = aim.takeDamage(dmg);

		//��Ϣ��ʽ
		msg += to_string(aim.hp()) + " 1 1 1 ";//������Ѫ����atk(1���)��def(1)��speed��1��
//		for (int i = 0; i < 3; ++i)
//		{
//			msg += to_string(aim.cpp(i)) + ' ';
//		}
		msg += to_string(attacker.hp()) + " 1 1 1 ";//������Ѫ����atk(1��ʾ����)��def(1)��speed��1��
//		for (int i = 0; i < 3; ++i)
//		{
//			msg += to_string(attacker.cpp(i)) + ' ';
//		}
		return result;

		break;
	}
	case 2:				 //���泵
		msg += "0 "; //����

		attacker.changeAtk(attacker.atk() / 8);
		msg += to_string(aim.hp()) + " 1 1 1 ";
//		for (int i = 0; i < 3; ++i)
//		{
//			msg += to_string(aim.cpp(i)) + ' ';
//		}
		msg += to_string(attacker.hp()) + " 2 1 1 ";//(2��ʾ����)
//		for (int i = 0; i < 3; ++i)
//		{
//			msg += to_string(attacker.cpp(i)) + ' ';
//		}
		break;
	case 3: //�����ײ
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() * 1.5 - aim.cdef() + 8 + f(4 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(aim.cpp(i)) + ' ';
	//	}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		return result;

		break;
	}
	default://��ʼ���ܣ�ץ
	{
		
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + f(4);
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
		//{
		//	msg += to_string(aim.cpp(i)) + ' ';
	//	}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		return result;

		break;
	}
	} //switch
	return false;
}


///////////////////////////////////////////////////////////////////// 		
// function��		
//		Race()
// Description:
//		���������ļ���
/////////////////////////////////////////////////////////////////////
template <>
Race<1>::Race() : PokemonBase(HP)
{
	// _raceName = "bubuhua";
	_raceName = "shumiaogui";
	_expCurve[0] = 5;
	for (int i = 1; i < 14; ++i)
	{
		_expCurve[i] = _expCurve[i - 1] + 5 * i;
	}
	_skillName[0] = "zhuangji";
	_skillName[1] = "guanghezuoyong";
	_skillName[2] = "chaojixiqu";
	_skillName[3] = "feiyefengbao";

}

template <>
bool Race<1>::attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex) const
{
	// dbout << attacker.name() << " uses " << attacker.skillName(skillIndex) << "!\n";

	msg += attacker.skillName(skillIndex) + ' ';

	switch (skillIndex)
	{
	case 1: //�������
	{
		msg += "0 "; // can not dodge

		attacker.changeHp(attacker.catk() / 2 + attacker.cdef() + f(4));
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(aim.cpp(i)) + ' ';
	//	}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		break;
	}
	case 2: //������ȡ
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() + f(4 + attacker.lv());
		if (dmg < 10)
			dmg = 10;
		attacker.changeHp(dmg);
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(aim.cpp(i)) + ' ';
	//	}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
		//	msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		return result;
		break;
	}
	case 3: //��Ҷ�籩
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() * 2 - aim.cdef() + f(3 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(aim.cpp(i)) + ' ';
		//}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		return result;

		break;
	}
	default:
	{
		//��ʼ���ܣ�ײ��
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + f(4);
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
		//{
		//	msg += to_string(aim.cpp(i)) + ' ';
		//}
		msg += to_string(attacker.hp()) + " 1 1 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		return result;

		break;
	}
	} //switch
	return false;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		Race()
// Description:
//		����Ī�ȵļ���
/////////////////////////////////////////////////////////////////////
template <>
Race<2>::Race() : PokemonBase(DEF)
{
	// _raceName = "mobi";
	_raceName = "mobi";
	_expCurve[0] = 5;
	for (int i = 1; i < 14; ++i)
	{
		_expCurve[i] = _expCurve[i - 1] + 5 * i;
	}
	_skillName[0] = "chongding";
	_skillName[1] = "dadizhili";
	_skillName[2] = "shachenbao";
	_skillName[3] = "baoliequan";

}

template <>
bool Race<2>::attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex) const
{
	// dbout << attacker.name() << " uses " << attacker.skillName(skillIndex) << "!\n";

	msg += attacker.skillName(skillIndex) + ' ';

	switch (skillIndex)
	{
	case 1:  //���֮��
	{
		msg += "0 "; // can not dodge

		attacker.changeDef(2);
		attacker.changeAtk(2);
		// bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		//for (int i = 0; i < 3; ++i)
	//	{
		//	msg += to_string(aim.cpp(i)) + ' ';
	//	}
		msg += to_string(attacker.hp()) + " 2 2 1 ";
	//	for (int i = 0; i < 3; ++i)
	//	{
	//		msg += to_string(attacker.cpp(i)) + ' ';
	//	}
		// return result;
		break;
	}
	case 2: //ɳ����
	{
		attacker.changeAtk(2);
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + f(4 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		msg += to_string(attacker.hp()) + " 2 1 1 ";

		return result;

		break;
	}
	case 3: //����ȭ
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() * 2 - aim.cdef() + f(3 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		msg += to_string(attacker.hp()) + " 1 1 1 ";
		return result;

		break;
	}
	default:
	{
		//��ʼ���ܣ��嶥
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + f(4);
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		msg += to_string(attacker.hp()) + " 1 1 1 ";
		return result;
		break;
	}
	} //switch
	return false;
}

///////////////////////////////////////////////////////////////////// 		
// function��		
//		Race()
// Description:
//		���ò��˶��ļ���
/////////////////////////////////////////////////////////////////////
template <>
Race<3>::Race() : PokemonBase(SPE)
{
	// _raceName = "bokeer";
	_raceName = "bokeer";
	_expCurve[0] = 5;
	for (int i = 1; i < 14; ++i)
	{
		_expCurve[i] = _expCurve[i - 1] + 5 * i;
	}
	_skillName[0] = "zhaungji";
	_skillName[1] = "dianguanghuoshi";
	_skillName[2] = "yanfan";
	_skillName[3] = "mengqin";

}

template <>
bool Race<3>::attack(Pokemon& attacker, Pokemon& aim, string& msg, int skillIndex) const
{
	// dbout << attacker.name() << " uses " << attacker.skillName(skillIndex) << "!\n";

	msg += attacker.skillName(skillIndex) + ' ';

	switch (skillIndex)
	{
	case 1: //����ʯ
	{
		msg += "0 "; // can not dodge

		attacker.changeSpeed(attacker.speed() / 5);
		// bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		
		msg += to_string(attacker.hp()) + " 1 1 2 ";
		
		// return result;
		break;
	}
	case 2: //�෵
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() + attacker.cspeed() / 4 - aim.cdef() + f(4 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";
		
		msg += to_string(attacker.hp()) + " 1 1 1 ";
		
		return result;

		break;
	}
	case 3: //����
	{
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + attacker.cspeed() / 2 + f(3 + attacker.lv());
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";

		msg += to_string(attacker.hp()) + " 1 1 1 ";

		return result;

		break;
	}
	default:
	{
		//��ʼ���ܣ�ײ��
		if (dodge(attacker.cspeed(), aim.cspeed(), msg))
			return false;

		int dmg = attacker.catk() - aim.cdef() + f(4);
		bool result = aim.takeDamage(dmg);
		msg += to_string(aim.hp()) + " 1 1 1 ";

		msg += to_string(attacker.hp()) + " 1 1 1 ";

		return result;

		break;
	}
	} //switch
	return false;
}
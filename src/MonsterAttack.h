#pragma once
#include "MonsterParent.h"
class Attack {
public:
	Attack(); 
	
	~Attack();
	std::string Type();
	std::string Name();
	int			Count();
	int			Index();
	int			MaxHp();
	int			Speed();
	int			WGold();
	int			MGold();
	void LoadTowerFormXML(rapidxml::xml_node<>* atkNode);
		
private:
	int _index;
	std::string _monsterType;
	std::string _attackName;
	int		    _monsterCount;
	int         _maxHp;
	int         _modSpeed;
	int         _waveGold;
	int         _monsterGold;
};

class MonsterAttack {
public:
	MonsterAttack();
	~MonsterAttack();
	void SetDelay(float d);
	
	void LoadFromXml(std::string &);
	float Delay();
	std::vector<Attack> & GetAttack();
	std::vector<MonsterParent::Ptr> & GetAttackPrototypes();
private:
	std::vector<Attack> _attacks;
	std::vector<MonsterParent::Ptr> _attackPrototypes;
	float _attackDelay;
};



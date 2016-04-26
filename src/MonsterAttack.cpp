#include "stdafx.h"
#include "MonsterParent.h"
#include "MonsterAttack.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace rapidxml;

Attack::Attack(){
	_index = 0;
	_monsterType = "";
	_attackName = "";
	_monsterCount = 0;
	_maxHp = 0;
	_modSpeed = 0;
	_waveGold = 0;
	_monsterGold = 0;
};

Attack::~Attack() {};

std::string Attack::Type() {
	return _monsterType;
};

std::string Attack::Name() {
	return _attackName;
};

int	Attack::Count() {
	return _monsterCount;
};

int Attack::Index() {
	return _index;
};

int Attack::MaxHp() {
	return _maxHp;
};

int	Attack::Speed() {
	return _modSpeed;
};

int Attack::WGold() {
	return _waveGold;
};

int Attack::MGold() {
	return _monsterGold;
};


void Attack::LoadTowerFormXML(rapidxml::xml_node<>* atkNode) {
	_attackName = Xml::GetStringAttributeOrDef(atkNode, "name", "name");
	_monsterType = Xml::GetStringAttributeOrDef(atkNode, "type", "Normal");
	_monsterCount = Xml::GetIntAttributeOrDef(atkNode, "count", 0);
	_maxHp = Xml::GetIntAttributeOrDef(atkNode, "hp", 0);
	_modSpeed = Xml::GetIntAttributeOrDef(atkNode, "speed", 0);
	_monsterGold = Xml::GetIntAttributeOrDef(atkNode, "goldPM", 0);
	_waveGold = Xml::GetIntAttributeOrDef(atkNode, "goldAA", 0);
};











MonsterAttack::MonsterAttack() {
	_attacks.clear();
	_attackDelay = 0;
};

MonsterAttack::~MonsterAttack() {};

void MonsterAttack::SetDelay(float d) {
	_attackDelay = d;
};



void MonsterAttack::LoadFromXml(std::string & filename) {
	_attacks.clear();
	_attackDelay = 0;
	try {
		file<> file(filename.c_str());
		// Может бросить исключение, если нет файла.

		xml_document<> doc;
		doc.parse<0>(file.data());
		// Может бросить исключение, если xml испорчен.

		xml_node<>* game = doc.first_node();
		if (!game) { Assert(false); throw runtime_error("No root node"); }

		xml_node<>* attacks = game->first_node("Attacks");
		string delay = attacks->first_attribute("delay")->value();
		_attackDelay = utils::lexical_cast<float>(delay);
		for (xml_node<>* attack = attacks->first_node("Attack"); attack; attack = attack->next_sibling("Attack")) {
			
			Attack atk;
			atk.LoadTowerFormXML(attack);
			MonsterParent::MonsterInfo info;
			string value;
			info._position = FPoint(0, 0);
			info._hp = atk.MaxHp();
			info._modSpeed = atk.Speed();
			info._map = nullptr;
			info._meat = atk.MGold();

			value = attack->first_attribute("runAnimation")->value();
			info._runAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

			value = attack->first_attribute("idleAnimation")->value();
			info._idleAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

			value = attack->first_attribute("dieAnimation")->value();
			info._dieAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
						
			info._dieSound = Xml::GetStringAttributeOrDef(attack, "dieSound", "Die");
			
			if (atk.Type() == "Normal") {
				_attackPrototypes.push_back(new NormalMonster(info));
			}
			else if (atk.Type() == "Immune") {
				_attackPrototypes.push_back(new ImmuneMonster(info));
			}
			else if (atk.Type() == "Healing") {
				info._healPerSecond = 150 + 1 * _attacks.size();
				_attackPrototypes.push_back(new HealingMonster(info));
			}
			else if (atk.Type() == "Boss") {
				info._reduceDamage = 0.3 + 0.01 * _attacks.size();
				_attackPrototypes.push_back(new BossMonster(info));
			}
			_attacks.push_back(atk);
		}


	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}



};


float MonsterAttack::Delay() {
	return _attackDelay;
};


std::vector<Attack> & MonsterAttack::GetAttack() {
	return _attacks;
};

std::vector<MonsterParent::Ptr> & MonsterAttack::GetAttackPrototypes() {
	return _attackPrototypes;
};










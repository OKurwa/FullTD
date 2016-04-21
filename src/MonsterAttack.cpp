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
	_finished = false;
	_waveGold = 0;
	_monsterGold = 0;
};
Attack::Attack(int index, const std::string mType, const std::string aName, const int mCount, int hp, int speed) {
	_index = index;
	_monsterType = mType;
	_attackName = aName;
	_monsterCount = mCount;
	_maxHp = hp;
	_modSpeed = speed;
	_finished = false;
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

bool Attack::Finished() {
	return _finished;
};

void Attack::SetType(std::string s) {
	_monsterType = s;
};
void Attack::SetName(std::string s) {
	_attackName = s;
};
void Attack::SetCount(int count) {
	_monsterCount = count;
};
void Attack::SetIndex(int index) {
	_index = index;
};

void Attack::SetMaxHp(int hp) {
	_maxHp = hp;
};

void Attack::SetSpeed(int speed) {
	_modSpeed = speed;
};

void Attack::SetWGold(int g) {
	_waveGold = g;
};

void Attack::SetMGold(int g) {
	_monsterGold = g;
};

void Attack::SetFinished(bool finish) {
	_finished = finish;
};

MonsterAttack::MonsterAttack() {
	_attacks.clear();
	_attackDelay = 0;
};

MonsterAttack::~MonsterAttack() {};

void MonsterAttack::SetDelay(float d) {
	_attackDelay = d;
};

void MonsterAttack::LoadFromFile(std::string file) {
	_attacks.clear();
	std::ifstream settingsFile(file);
	std::string line;
	std::vector<std::string> lines;
	lines.clear();

	if (settingsFile.is_open()) {
		while (std::getline(settingsFile, line)) {
			if (line != "")
				lines.push_back(line);

		}
	}
	settingsFile.close();


	int index = 0;
	
	std::string name;
	std::string value;
	line = lines[0];
	size_t pos = line.find("=", 0);
	name = line.substr(0, pos);
	value = line.substr(pos + 1);
	if (name == "TYPE" && value == "MA") {
		line = lines[1];
		pos = line.find("=", 0);
		name = line.substr(0, pos);
		value = line.substr(pos + 1);
		if (name == "attackCount") {
			_attacks.resize(utils::lexical_cast<int>(value));
		}

		line = lines[2];
		pos = line.find("=", 0);
		name = line.substr(0, pos);
		value = line.substr(pos + 1);
		if (name == "delay") {
			_attackDelay = utils::lexical_cast<float>(value);
		}
		
		for (unsigned int i = 3; i < lines.size(); i++) {
			line = lines[i];
			pos = line.find("=", 0);
			name = line.substr(0, pos);
			value = line.substr(pos + 1);

			if (name == "index") {
				index = utils::lexical_cast<int>(value);
				_attacks[index].SetIndex(index);
			}
			if (name == "type") {
				_attacks[index].SetType(value);
			}

			if (name == "name") {
				_attacks[index].SetName(value);
			}
			if (name == "count") {
				_attacks[index].SetCount(utils::lexical_cast<int>(value));
			}
			if (name == "hp") {
				_attacks[index].SetMaxHp(utils::lexical_cast<int>(value));
			}
			if (name == "speed") {
				_attacks[index].SetSpeed(utils::lexical_cast<int>(value));
			}
			if (name == "wgold") {
				_attacks[index].SetWGold(utils::lexical_cast<int>(value));
			}
			if (name == "mgold") {
				_attacks[index].SetMGold(utils::lexical_cast<int>(value));
			}

		}
	}
};

void MonsterAttack::LoadFromXml(std::string filename) {
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
			string value = attack->first_attribute("name")->value();
			atk.SetName(value);
			value = attack->first_attribute("type")->value();
			atk.SetType(value);
			value = attack->first_attribute("count")->value();
			atk.SetCount(utils::lexical_cast<int>(value));
			value = attack->first_attribute("hp")->value();
			atk.SetMaxHp(utils::lexical_cast<int>(value));
			value = attack->first_attribute("speed")->value();
			atk.SetSpeed(utils::lexical_cast<int>(value));
			value = attack->first_attribute("goldPM")->value();
			atk.SetMGold(utils::lexical_cast<int>(value));
			value = attack->first_attribute("goldAA")->value();
			atk.SetWGold(utils::lexical_cast<int>(value));
			if (atk.Type() == "Normal") {
				NormalMonster::NormMInfo info;
				info._position = FPoint(0, 0);
				info._hp = atk.MaxHp();
				info._modSpeed = atk.Speed();
				info._map = nullptr;
				value = attack->first_attribute("runAnimation")->value();
				info._runAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				value = attack->first_attribute("idleAnimation")->value();
				info._idleAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				value = attack->first_attribute("dieAnimation")->value();
				info._dieAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				
				
				value = Xml::GetStringAttributeOrDef(attack, "dieSound", "Die");
				info._dieSound = value;
				info._meat = atk.MGold();
				_attackPrototypes.push_back(new NormalMonster(info));
			}
			if (atk.Type() == "Immune") {
				ImmuneMonster::ImmMInfo info;
				info._position = FPoint(0, 0);
				info._hp = atk.MaxHp();
				info._modSpeed = atk.Speed();
				info._map = nullptr;
				value = attack->first_attribute("runAnimation")->value();
				info._runAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("idleAnimation")->value();
				info._idleAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("dieAnimation")->value();
				info._dieAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				value = Xml::GetStringAttributeOrDef(attack, "dieSound", "Die");
				info._dieSound = value;
				info._meat = atk.MGold();
				_attackPrototypes.push_back(new ImmuneMonster(info));
			}
			if (atk.Type() == "Healing") {
				HealingMonster::HealMInfo info;
				info._position = FPoint(0, 0);
				info._hp = atk.MaxHp();
				info._modSpeed = atk.Speed();
				info._map = nullptr;
				info._healPerSecond = 150 + 1 * _attacks.size();
				value = attack->first_attribute("runAnimation")->value();
				info._runAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("idleAnimation")->value();
				info._idleAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("dieAnimation")->value();
				info._dieAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				value = Xml::GetStringAttributeOrDef(attack, "dieSound", "Die");
				info._dieSound = value;
				info._meat = atk.MGold();
				_attackPrototypes.push_back(new HealingMonster(info));
			}
			if (atk.Type() == "Boss") {
				BossMonster::BossMInfo info;
				info._position = FPoint(0, 0);
				info._hp = atk.MaxHp();
				info._modSpeed = atk.Speed();
				info._map = nullptr;
				info._reduceDamage = 0.3 + 0.01 * _attacks.size();
				value = attack->first_attribute("runAnimation")->value();
				info._runAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("idleAnimation")->value();
				info._idleAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();

				value = attack->first_attribute("dieAnimation")->value();
				info._dieAnim = Core::resourceManager.Get<Render::Animation>(value)->Clone();
				
				value = Xml::GetStringAttributeOrDef(attack, "dieSound", "Die");
				info._dieSound = value;
				info._meat = atk.MGold();
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






void MonsterAttack::SaveToFile(std::string file) {
	std::ofstream sFile(file);
	std::string line;
	std::vector<std::string> lines;
	lines.clear();
	lines.push_back("TYPE=MA");
	lines.push_back("");
	lines.push_back("attackCount=" + utils::lexical_cast(_attacks.size()));
	lines.push_back("delay=" + utils::lexical_cast(_attackDelay));
	lines.push_back("");
	for (unsigned int attack = 0; attack < _attacks.size(); attack++) {
		lines.push_back("index=" + utils::lexical_cast(_attacks[attack].Index()));
		lines.push_back("type=" + _attacks[attack].Type());
		lines.push_back("name=" + _attacks[attack].Name());
		lines.push_back("count=" + utils::lexical_cast(_attacks[attack].Count()));
		lines.push_back("");
	}

	if (sFile.is_open()) {
		for (unsigned int i = 0; i < lines.size(); i++) {
			sFile << lines[i] << endl;
		}
	}
	sFile.close();
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










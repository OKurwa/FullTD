#include "stdafx.h"
#include "World.h"


void World::SetTowerTypeById(int buttonId)
{
	_curTowerType = _shop.GetTypeFromButton(buttonId);
}

std::vector<int> World::GetDisabled()
{

	return _shop.GetDisabledButtons(_info.gold);
}

void World::SetTowerTypeByType(TowerType type)
{
	_curTowerType = type;
}

int World::EarlyStart()
{
	int gold = _info.delayTimer;
	_info.delayTimer = 0;
	return gold;
}

World::World() {
	
};
World::~World() {};


void World::Init(	int gold,
			int attacksRemaining,
			int nextAttackMonstersCount,
			int monstersRemaining,
			int lives,
			MonsterAttack & attacks) {
	_info.gold = gold;
	_info.attacksRemaining = attacksRemaining;
	_nextAttackMonstersCount = nextAttackMonstersCount;
	_monstersRemaining = monstersRemaining;
	_info.HP = lives;
	_info.curAttack = 0;
	//_attacks = attacks;
	if (attacks.GetAttack().size() > 0) {
		_info.name = attacks.GetAttack()[_info.curAttack].Name();
		
		_info.type = attacks.GetAttack()[_info.curAttack].Type();
		_state = START;
	}else{
		_state = WIN;
	}
	std::string towerFile = "Towers.xml";
	_shop.LoadFromXml(towerFile);
	_curTowerType = EMPTY;
	Message msg = Message("ChangeGold", 1);
	msg.SetTargetLayer("TestLayer");
	Core::mainScreen.ProcessMessage(msg);
	_info.delayTimer = attacks.Delay();
};


GameState World::State() {
	return _state;
};



World::WorldInfo World::GetInfo() {
	return _info;
};

int World::Gold() {
	return _info.gold;

};

void World::StartNewAttack(float delay, Attack & atk) {
	
	if(_info.attacksRemaining>0){
		_state = DELAY;
		_info.delayTimer = delay;
		GoldAdd(atk.WGold());
		
		Message msg = Message("ChangeGold", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		
		++_info.curAttack;
		--_info.attacksRemaining;
	}
	else{
		if(_info.HP > 0)
			_state = WIN;
	}
	
	_info.name = atk.Name();
	_info.name = atk.Type();
	_info.monsterMaxHP = atk.MaxHp();
	_info.monsterSpeed = atk.Speed();
	_info.monsterMeat = atk.MGold();
		
};

void World::Update(float dt) {
	
	if (_info.HP <= 0)
		_state = LOSE;
	if (_info.HP == 0 && _info.attacksRemaining == 0 && _nextAttackMonstersCount == 0 && _monstersRemaining == 0)
		_state = START;
	
	if (_state == DELAY) {
		if (_info.delayTimer > 0) {
			_info.delayTimer -= dt;
		}
		else {
			_info.delayTimer = 0;
			_state = WAVE;
		}
	}

};
void World::GoldAdd(int gold) {
	_info.gold += gold;
	Message msg = Message("ChangeGold", 1);
	msg.SetTargetLayer("TestLayer");
	Core::mainScreen.ProcessMessage(msg);
};

bool World::GoldSpend(int gold) {
	if (gold > _info.gold) {
		return false;
	}
	else{
		_info.gold -= gold;
		Message msg = Message("ChangeGold", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		return true;
	}
};

void World::LoseLife(int i) {
	_info.HP-=i;
}
TowerType World::GetTowerType()
{
	return _curTowerType;
}
int World::GetButtonId()
{
	return _shop.GetButtonFromType(_curTowerType);
}
;

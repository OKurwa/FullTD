#include "stdafx.h"
#include "World.h"


void World::SetTowerTypeById(int buttonId)
{
	_curTowerType = _shop.GetTypeFromButton(buttonId);
}

std::vector<int> World::GetDisabled()
{

	return _shop.GetDisabledButtons(_gold);
}

void World::SetTowerTypeByType(TowerType type)
{
	_curTowerType = type;
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
	_gold = gold;
	_attacksRemaining = attacksRemaining;
	_nextAttackMonstersCount = nextAttackMonstersCount;
	_monstersRemaining = monstersRemaining;
	_lives = lives;
	_curAttackIndex = 0;
	//_attacks = attacks;
	if (attacks.GetAttack().size() > 0) {
		_curAttackName = attacks.GetAttack()[_curAttackIndex].Name();
		
		_curAttackType = attacks.GetAttack()[_curAttackIndex].Type();
		_state = START;
	}else{
		_state = WIN;
	}
	_shop.LoadFromXml("Towers.xml");
	_curTowerType = EMPTY;
	Message msg = Message("ChangeGold", 1);
	msg.SetTargetLayer("TestLayer");
	Core::mainScreen.ProcessMessage(msg);
};


GameState World::State() {
	return _state;
};



World::WorldInfo World::GetInfo() {
	WorldInfo info;
	info.attacksRemaining = _attacksRemaining;
	info.curAttack = _curAttackIndex;
	info.delayTimer = _delayTimer;
	info.gold = _gold;
	info.HP = _lives;
	info.monsterMaxHP = _maxHP;
	info.monsterMeat = _gpm;
	info.monsterSpeed = _speed;
	info.name = _curAttackName;
	info.type = _curAttackType;
	return info;
};

int World::Gold() {
	return _gold;

};

void World::StartNewAttack(float delay, Attack & atk) {
	
	if(_attacksRemaining>0){
		_state = DELAY;
		_delayTimer = delay;
		GoldAdd(atk.WGold());
		
		Message msg = Message("ChangeGold", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		
		++_curAttackIndex;
		--_attacksRemaining;
	}
	else{
		if(_lives > 0)
			_state = WIN;
	}
	
	_curAttackName = atk.Name();
	_curAttackType = atk.Type();
	_maxHP = atk.MaxHp();
	_speed = atk.Speed();
	_gpm = atk.MGold();
		
};

void World::Update(float dt) {
	
	if (_lives <= 0)
		_state = LOSE;
	if (_lives == 0 && _attacksRemaining == 0 && _nextAttackMonstersCount == 0 && _monstersRemaining == 0)
		_state = START;
	
	if (_state == DELAY) {
		if (_delayTimer > 0) {
			_delayTimer -= dt;
		}
		else {
			_delayTimer = 0;
			_state = WAVE;
		}
	}

};
void World::GoldAdd(int gold) {
	_gold += gold;
	Message msg = Message("ChangeGold", 1);
	msg.SetTargetLayer("TestLayer");
	Core::mainScreen.ProcessMessage(msg);
};

bool World::GoldSpend(int gold) {
	if (gold > _gold) {
		return false;
	}
	else{
		_gold -= gold;
		Message msg = Message("ChangeGold", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		return true;
	}
};

void World::LoseLife(int i) {
	_lives-=i;
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

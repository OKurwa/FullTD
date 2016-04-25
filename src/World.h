#pragma once
#include "MonsterAttack.h"
#include "TowerParent.h"
#include "TowerShop.h"

enum GameState {
	START,
	LOSE,
	WIN,
	DELAY,
	WAVE,
	PAUSE
};

class World
{
public:
	static World & Instance()
	{
		static World theSingleInstance;
		return theSingleInstance;
	}
	~World();
	struct WorldInfo {
		int HP;
		float delayTimer;
		int attacksRemaining;
		int curAttack;
		std::string name;
		std::string type;
		int monsterMaxHP;
		int monsterSpeed;
		int monsterMeat;
		int gold;
	};
	void Init(	int gold,
				int attacksRemaining,
				int nextAttackMonstersCount,
				int monstersRemaining,
				int lives,
				MonsterAttack & attacks);
	
	WorldInfo GetInfo();

	GameState State();
	void StartNewAttack(float delay, Attack & atk);
	int Gold();
	void Update(float dt);
	void GoldAdd(int gold);
	bool GoldSpend(int gold);
	void LoseLife(int);
	TowerType GetTowerType();
	int GetButtonId();
	void SetTowerTypeById(int buttonId);
	std::vector<int> GetDisabled();
	void SetTowerTypeByType(TowerType);
	int EarlyStart();
private:
	World();
	World(const World& root);
	World& operator =(const World&);
	
	TowerType _towerType;
	
	int _gold;
	int _attacksRemaining;
	int _nextAttackMonstersCount;
	int _monstersRemaining;
	int _lives;
	int _curAttackIndex;
	TowerType _curTowerType;
	int _maxHP;
	int _speed;
	int _gpm;
	std::string _curAttackName;
	std::string _curAttackType;
	float _delayTimer;
	GameState _state;
	TowerShop _shop;
	//MonsterAttack _attacks;
};
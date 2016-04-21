#pragma once
#include "MonsterAttack.h"
#include "TowerParent.h"

enum WorldState {
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
	
	void Init(	int gold,
				int attacksRemaining,
				int nextAttackMonstersCount,
				int monstersRemaining,
				int lives,
				MonsterAttack & attacks);
	std::string CurAttackName();
	std::string NextAttackName();
	std::string CurAttackType(); 
	std::string NextAttackType();
	int Gold();
	WorldState State();

	void SetState(WorldState);
	void SetAttackIndex(int index);
	void SetNewAttack(float delay, Attack & atk);
	void Draw();
	void Update(float dt);
	void GoldAdd(int gold);
	bool GoldSpend(int gold);
	void LoseLife(int);
	
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
	int _maxHP;
	int _speed;
	int _gpm;
	std::string _curAttackName;
	std::string _nextAttackName;
	std::string _curAttackType;
	std::string _nextAttackType;
	float _delayTimer;
	WorldState _state;
	Render::TexturePtr _start_w;
	Render::TexturePtr _lose_w;
	Render::TexturePtr _win_w;
	//MonsterAttack _attacks;
};
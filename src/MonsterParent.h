#pragma once
#include "FieldMap.h"

class MonsterParent : public RefCounter
{
public:
	typedef boost::intrusive_ptr<MonsterParent> Ptr;
	struct MonsterInfo{
		FPoint				 _position;
		int					 _modSpeed;
		int					 _hp;
		FieldMap *			 _map;
		Render::AnimationPtr _runAnim;
		Render::AnimationPtr _idleAnim;
		Render::AnimationPtr _dieAnim;
		std::string			 _dieSound;
		int					 _meat;
		float				 _reduceDamage;
		int					 _healPerSecond;
	};


	MonsterParent();
	MonsterParent(FPoint position, int modSpeed, int hp, FieldMap * map, Render::TexturePtr _skin);
	virtual ~MonsterParent();

	virtual void Draw();
	void DrawMeat();
	virtual void Update(float dt);
	virtual void PostUpdate(float dt) {};
	void UpdateAnimAngle(float dt);
	virtual void TakeDamage(TowerType effType, FPoint values, float damage) = 0;
	virtual Ptr clone() = 0;
	bool FindAWay();
	std::vector<IPoint> FillAround(std::vector<IPoint> lastWaveFilled, std::vector<std::vector<int>> & map, int d);
	bool Dead();
	bool Dying();
	bool EndDeadAnim();
	bool Finish();
	float WayDistance();
	FPoint Position();
	FPoint HitPosition(float dt);
	void SetPosition(FPoint,FieldMap*);
	int Damage() { return _damage; };
	int TakeMonsterMeat();
	
protected:
	int ref_cnt_;
	int _damage;
	FPoint	   _position;
	FPoint	   _hitPosition;
	FPoint	   _speed;
	int		   _modSpeed;
	float	   _hp;
	int		   _meatCont;
	float	   _maxHp;
	bool	   _dead; 
	bool	   _dying;
	bool       _finish;
	bool	   _empty;
	FieldMap   * _map;
	IPoint _curCell;
	float _moveTimer;
	float _curWayDistance;
	std::vector<IPoint> _currentWay;
	Render::TexturePtr _skin; 
	
	//EffectsContainer _boomCont;
	//ParticleEffectPtr _boomEff;
	TimedSplinePath _curWaySplineX;
	TimedSplinePath _curWaySplineY;

	//Ёффекты атак и счетчики

	FPoint _bash;
	FPoint _decay;
	FPoint _slow;
	
	//јнимации
	Render::AnimationPtr _runAnim;
	Render::AnimationPtr _idleAnim;
	Render::AnimationPtr _dieAnim;

	Render::TexturePtr _meat;

	AnimAngles _runAnimAngles;
	AnimAngles _idleAnimAngles;
	AnimAngles _dieAnimAngles;
	float _lastAngle;

	bool _hint;
	std::string _dieSound;
};




class BossMonster : public MonsterParent
{
public:
	BossMonster();
	BossMonster(BossMonster& proto);
	BossMonster(MonsterParent::MonsterInfo);
	~BossMonster();
	MonsterParent::Ptr clone() {

		return new BossMonster(*this);
	}
	//void Draw();
	//void Update(float dt);
	void TakeDamage(TowerType effType, FPoint values, float damage);
	//void PostUpdate(float dt);
private:
	float _reduceDamage;
};

class ImmuneMonster : public MonsterParent
{
public:
	ImmuneMonster();
	ImmuneMonster(ImmuneMonster& proto);
	ImmuneMonster(MonsterParent::MonsterInfo);
	~ImmuneMonster();
	MonsterParent::Ptr clone() {

		return new ImmuneMonster(*this);
	}
	//void Draw();
	//void Update(float dt);
	
	void TakeDamage(TowerType effType, FPoint values, float damage);
	//void PostUpdate(float dt);
private:

};

class HealingMonster :public MonsterParent
{
public:
	HealingMonster();
	HealingMonster(HealingMonster& proto);
	HealingMonster(MonsterParent::MonsterInfo);
	~HealingMonster();
	MonsterParent::Ptr clone() {

		return new HealingMonster(*this);
	}
	//void Draw();
	//void Update(float dt);
	void TakeDamage(TowerType effType, FPoint values, float damage);
	void PostUpdate(float dt);
private:
	float _healPerSecond;
};

class NormalMonster :public MonsterParent
{
public:
	NormalMonster();
	NormalMonster(NormalMonster& proto);
	NormalMonster(MonsterParent::MonsterInfo);
	~NormalMonster();
	MonsterParent::Ptr clone() {

		return new NormalMonster(*this);
	}
	//void Draw();
	//void Update(float dt);
	void TakeDamage(TowerType effType, FPoint values, float damage);
	//void PostUpdate(float dt);
private:
	
};


const AnimAngles MOB_IDL_ANGLES = {
	IPoint(0,3),
	IPoint(20,23),
	IPoint(40,43),
	IPoint(60,63),
	IPoint(80,83),
	IPoint(100,103),
	IPoint(120,123),
	IPoint(140,143) };

const AnimAngles RUN_ANGLES = {
	IPoint(4, 11),
	IPoint(24, 31),
	IPoint(44, 51),
	IPoint(64, 71),
	IPoint(84, 91),
	IPoint(104, 111),
	IPoint(124, 131),
	IPoint(144, 151) };

const AnimAngles DIE_ANGLES = {
	IPoint(12,19),
	IPoint(32,39),
	IPoint(52,59),
	IPoint(72,79),
	IPoint(92,99),
	IPoint(112,119),
	IPoint(132,139),
	IPoint(152,159) };

const Sector angle0 = {-22.5, 22.5};
const Sector angle45 = { 22.5, 67.5 };
const Sector angle90 = { 67.5, 112.5 };
const Sector angle135 = { 112.5, 157.5 };
const Sector angle180 = { 157.5, -157.5 };
const Sector angle225 = { -157.5, -112.5 };
const Sector angle270 = { -112.5, -67.5 };
const Sector angle315 = { -67.5, -22.5 };

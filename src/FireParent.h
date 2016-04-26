#pragma once
#include "MonsterParent.h"

//////////////////////////////
//--------------------------//
//----------Снаряды---------//
//--------------------------//
//////////////////////////////
//----------------------------------------------//
//----------------------------------------------//
//			Базовый класс снаряда				//
//----------------------------------------------//
//----------------------------------------------//



class FireParent : public RefCounter{
public:
	typedef boost::intrusive_ptr<FireParent> Ptr;
	struct MissInfo {
		FPoint _position;
		IPoint _damage;
		int	_modSpeed;
		MonsterParent * _target;
		FPoint _tPosition;
		int _price;

		//Эффекты атак
		FPoint _sFactor;
		FPoint _decay;
		FPoint _bash;

		//Радиус поражения
		int _sRange;

	};
	
	FireParent();	
	
	~FireParent() {};
	
	void Draw();

	void Update(float dt);

	virtual void DealDamage();

	//virtual Ptr Clone() = 0;

	void SetPosition(FPoint);

	TowerType Type();

	bool Hit();

	void MakePath();

	void TakeNearestMonster(float & distance, int & index, std::vector<MonsterParent::Ptr> & monsters);

	virtual MonsterParent::Ptr TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range);

protected:
	int ref_cnt_;
	TowerType _missileType;
	FPoint _position;
	FPoint _targetPosition;
	FPoint _speed;
	int	   _modSpeed;
	float  _flyTime;
	float  _missileTimer;
	bool   _fly;
	bool   _hit;
	int _price;
	TimedSplinePath _missilePathX;
	TimedSplinePath _missilePathY;
	Render::TexturePtr _tex;
	IPoint _damage;
	MonsterParent * _target;

	EffectsContainer _misEffCont;
	ParticleEffectPtr _misEff;
	ParticleEffectPtr _hitEff;
};



//----------------------------------------------//
//----------------------------------------------//
//				Обычный снаряд					//
//----------------------------------------------//
//----------------------------------------------//


class NormalMissile : public FireParent {
public:
	
	NormalMissile();
	
	NormalMissile(MissInfo);
	
	~NormalMissile();
	
	void DealDamage() override;
	
	
	
	
	
private:
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Замедляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//
class SlowMissile : public FireParent {
public:
	
	SlowMissile();
	
	SlowMissile(MissInfo, std::vector<MonsterParent::Ptr> & targets);

	~SlowMissile();

	void DealDamage() override;
	
	MonsterParent::Ptr TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) override;
private:
	FPoint _slow;
	int _splashRange;
	std::vector<MonsterParent::Ptr> _targets;
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Отравляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//
class DecayMissile : public FireParent {
public:
	
	DecayMissile();
	
	DecayMissile(MissInfo);

	~DecayMissile();

	void DealDamage() override;
	
	
	
private:
	
	FPoint _decay;
	
 };

//----------------------------------------------//
//----------------------------------------------//
//				Оглушающий снаряд				//
//----------------------------------------------//
//----------------------------------------------//
class BashMissile : public FireParent {
public:
	
	BashMissile();
	
	BashMissile(MissInfo);

	~BashMissile();

	void DealDamage() override;
	
	
	
private:
	
	FPoint _bash;
};


//----------------------------------------------//
//----------------------------------------------//
//				Разрывной снаряд				//
//----------------------------------------------//
//----------------------------------------------//
class SplashMissile : public FireParent {
public:
	
	SplashMissile();
	
	SplashMissile::SplashMissile(MissInfo inf, std::vector<MonsterParent::Ptr> & targets);

	~SplashMissile();

	void DealDamage() override;
	
	
	MonsterParent::Ptr TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) override;
private:
	int _splashRange;
	std::vector<MonsterParent::Ptr> _targets;
	
};













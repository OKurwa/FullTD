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



class FireParent {
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

	FireParent(FPoint position,FPoint tPosition, int mSpeed, float fTime, float mFlyTimer, TowerType mType, IPoint dmg, Render::TexturePtr tex);
	
	~FireParent() {};
	
	void Draw();

	void Update(float dt);

	virtual void DealDamage();

	virtual Ptr clone() = 0;
	
	FPoint Position();

	FPoint TargetPosition();

	void SetPosition(FPoint);

	TowerType Type();

	bool Fly();

	bool Hit();

	void MakePath();

	virtual MonsterParent::Ptr TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range);

	friend void intrusive_ptr_add_ref(FireParent*);
	friend void intrusive_ptr_release(FireParent*);

	void AddRef() {
		++ref_cnt_;
	}

	void Release() {
		if (--ref_cnt_ == 0) {
			delete this;

		}
	}
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
inline void intrusive_ptr_add_ref(FireParent* e) { e->AddRef(); }
inline void intrusive_ptr_release(FireParent* e) { e->Release(); }


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
	
	NormalMissile::Ptr clone() {
		return new NormalMissile(*this);
	}
	
	void SetTarget(MonsterParent * target);
	
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
	
	FireParent::Ptr clone() {
		return new SlowMissile(*this);
	}
	
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
	
	FireParent::Ptr clone() {
		return new DecayMissile(*this);
	}
	
private:
	
	FPoint _decay;
	//FPoint decay;
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
	
	FireParent::Ptr clone() {
		return new BashMissile(*this);
	}
	
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
	
	FireParent::Ptr clone() {
		return new SplashMissile(*this);
	}
	
	MonsterParent::Ptr TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) override;
private:
	int _splashRange;
	std::vector<MonsterParent::Ptr> _targets;
	
};













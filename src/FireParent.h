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
	struct NMissInfo {
		FPoint _position;
		int	_modSpeed;
		IPoint _damage;
		MonsterParent * _target;
		int _price;
	};

	NormalMissile();
	
	NormalMissile(NMissInfo);
	
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
	struct SlMissInfo {
		FPoint _position;
		FPoint _tPosition;
		int	_modSpeed;
		IPoint _damage;
		FPoint _sFactor;
		int _sRange;
		int _price;
	};


	SlowMissile();
	
	SlowMissile(SlMissInfo, std::vector<MonsterParent::Ptr> & targets);

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
	struct DMissInfo {
		FPoint _position;
		int	_modSpeed;
		IPoint _damage;
		MonsterParent * _target;
		FPoint _decay;
		int _price;
	};
	DecayMissile();
	
	DecayMissile(DMissInfo);

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
	struct BMissInfo {
		FPoint _position;
		int	_modSpeed;
		IPoint _damage;
		MonsterParent * _target;
		FPoint _bash;
		int _price;
	};
	BashMissile();
	
	BashMissile(BMissInfo);

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
	struct SpMissInfo {
		FPoint _position;
		FPoint _tPosition;
		int	_modSpeed;
		IPoint _damage;
		int _sRange;
		int _price;
	};
	SplashMissile();
	
	SplashMissile::SplashMissile(SpMissInfo inf, std::vector<MonsterParent::Ptr> & targets);

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



// Фабрика для создания снарядов
class MissilePrototypeFactory
{
public:
	MissilePrototypeFactory() {
		_Nloaded = false;
		_Slloaded = false;
		_Sploaded = false;
		_Bloaded = false;
		_Dloaded = false;
	}
	FireParent::Ptr createNormal() {
		static NormalMissile prototype;
		if (!_Nloaded) {
			//prototype.LoadFromXml("NewMap.xml");
			_Nloaded = true;
		}

		return prototype.clone();
	}
	FireParent::Ptr createSlow() {
		static SlowMissile prototype;
		if (!_Slloaded) {
			//prototype.LoadFromXml("NewMap.xml");
			_Slloaded = true;
		}
		return prototype.clone();
	}
	FireParent::Ptr createSplash() {
		static SplashMissile prototype;
		if (!_Sploaded) {
			//prototype.LoadFromXml("NewMap.xml");
			_Sploaded = true;
		}
		return prototype.clone();
	}
	FireParent::Ptr createBash() {
		static BashMissile prototype;
		if (!_Bloaded) {
			//prototype.LoadFromXml("NewMap.xml");
			_Bloaded = true;
		}
		return prototype.clone();
	}
	FireParent::Ptr createDecay() {
		static DecayMissile prototype;
		if (!_Dloaded) {
			//prototype.LoadFromXml("NewMap.xml");
			_Dloaded = true;
		}
		return prototype.clone();
	}

private:
	bool _Nloaded;
	bool _Slloaded;
	bool _Sploaded;
	bool _Bloaded;
	bool _Dloaded;
};









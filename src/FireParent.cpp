#include "stdafx.h"
#include "FireParent.h"

using namespace std;
using namespace rapidxml;

//----------------------------------------------//
//----------------------------------------------//
//			Базовый класс снаряда				//
//----------------------------------------------//
//----------------------------------------------//

FireParent::FireParent() {
	//_missileType = "";
	_position = FPoint(0,0);
	_targetPosition = FPoint(0, 0);
	_speed = FPoint(0, 0);
	_modSpeed = 0;
	_flyTime = 0;
	_missileTimer = 0;
	_fly = true;
	_hit = false;
	_missilePathX.Clear();
	_missilePathY.Clear();
	_tex = nullptr;
	_damage = IPoint(0, 0);
};

FireParent::FireParent(FPoint position, FPoint tPosition, int mSpeed, float fTime, float mFlyTimer, TowerType mType, IPoint dmg, Render::TexturePtr tex) : ref_cnt_(0) {
	_missileType = mType;
	_position = position;
	_targetPosition = tPosition;
	_speed = FPoint(0, 0);
	_modSpeed = mSpeed;
	_flyTime = fTime;
	_missileTimer = mFlyTimer;
	_fly = true;
	_hit = false;
	_tex = tex;
	_damage = dmg;
	MakePath();

};

void FireParent::Draw() {
	if (_misEff || _hitEff) {

		_misEffCont.Draw();

	}
};

void FireParent::Update(float dt) {
	if (_missileTimer >= _flyTime && !_hit) {
		_hit = true;
		_fly = false;
		//
		DealDamage();
		//
		_misEff->Finish();
		if (_hitEff)
			_hitEff->Continue();

		_missileTimer = _flyTime;
	}
	else {
		if (!_hit)
			_missileTimer += dt;


		_position.x = _missilePathX.getGlobalFrame(_missileTimer);
		_position.y = _missilePathY.getGlobalFrame(_missileTimer);
		_misEff->posX = _position.x;
		_misEff->posY = _position.y;
	}
	if (_hitEff) {
		_hitEff->posX = _position.x;
		_hitEff->posY = _position.y;
	}
	_misEffCont.Update(dt);
};

void FireParent::DealDamage() {
};

FPoint FireParent::Position() { 
	return _position; 
};

FPoint FireParent::TargetPosition() {
	return _targetPosition;
};

void FireParent::SetPosition(FPoint pos) {
	_position = pos;
};

TowerType FireParent::Type() {
	return _missileType;

};

bool FireParent::isFlying() {
	return _fly;
};

bool FireParent::Hit() {
	if (_misEff && _hitEff) {



		if (_misEff->isEnd() && _hitEff->isEnd() &&_hit) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (_misEff) {



		if (_misEff->isEnd() && _hit) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return _hit;
	}
	
};

void FireParent::MakePath() {
	
	if (_flyTime == 0 && _missileTimer == 0 && _modSpeed > 0) {
		float distance = _position.GetDistanceTo(_targetPosition); 
		float time = distance / (float)_modSpeed;
		_flyTime = time;
		_missileTimer = 0;
		_missilePathX.Clear();
		_missilePathY.Clear();
		_missilePathX.addKey(0, _position.x);
		_missilePathY.addKey(0, _position.y);
		_missilePathX.addKey(time, _targetPosition.x);
		_missilePathY.addKey(time, _targetPosition.y);
	}
	else {
		_missilePathX.Clear();
		_missilePathY.Clear();
		_missilePathX.addKey(_missileTimer, _position.x);
		_missilePathY.addKey(_missileTimer, _position.y);
		_missilePathX.addKey(_flyTime, _targetPosition.x);
		_missilePathY.addKey(_flyTime, _targetPosition.y);
	}
	_missilePathX.CalculateGradient();
	_missilePathY.CalculateGradient();
	if (_misEff) {
		_misEff->posX = _position.x;
		_misEff->posY = _position.y;
		_misEff->Continue();
	}
	if (_hitEff) {
		_hitEff->posX = _position.x;
		_hitEff->posY = _position.y;
	}
		
};

void FireParent::TakeNearestMonster(float & distance, int & index, std::vector<MonsterParent::Ptr> & monsters) {
	for (unsigned int i = 0; i < monsters.size(); i++) {
		if (!monsters[i].get()->Dead() && !monsters[i].get()->Dying()) {
			FPoint tarPos = monsters[i]->Position();
			float tmpD = _position.GetDistanceTo(tarPos);
			if (tmpD<distance) {
				distance = tmpD;
				index = i;
			}
		}

	}
};

MonsterParent::Ptr  FireParent::TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) {
	MonsterParent::Ptr resTarget = nullptr;
	if (target == nullptr || target->Finish() || target->Dead() || target->Dying()) {
		float d = INFINITE;
		int tarIndex = INFINITE;
		TakeNearestMonster(d, tarIndex, monsters);
		
		if (d <= range && tarIndex < INFINITE) {
			resTarget = monsters[tarIndex];
			_target = resTarget.get();
			_flyTime = d / (float)_modSpeed;
			_targetPosition = monsters[tarIndex]->HitPosition(d / (float)_modSpeed);
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}
	else {
		FPoint tarPos = target->Position();
		float tmpD = _position.GetDistanceTo(tarPos);

		if (tmpD <= range) {
			resTarget = target;
			_target = resTarget.get();
			_flyTime = tmpD / (float)_modSpeed;
			_targetPosition = target->HitPosition(tmpD / (float)_modSpeed);
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}


};

//----------------------------------------------//
//----------------------------------------------//
//				Обычный снаряд					//
//----------------------------------------------//
//----------------------------------------------//
NormalMissile::NormalMissile() : FireParent() {
	_missileType = TowerType::NORMAL;
	_target = nullptr;
};

NormalMissile::NormalMissile(MissInfo inf) : FireParent() {
	_missileType = TowerType::NORMAL;
	_position = inf._position;
	
	_modSpeed = inf._modSpeed;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_damage = inf._damage;
	_target = inf._target;
	_misEff = _misEffCont.AddEffect("IskraViol");
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	MakePath();
};

NormalMissile::~NormalMissile() {

};

void NormalMissile::DealDamage()
{
	_target->TakeDamage(_missileType, FPoint(0, 0), math::random(_damage.x, _damage.y));
}

void NormalMissile::SetTarget(MonsterParent * target) {
	_target = target;
	if (target && _modSpeed>0) {
		float d = _position.GetDistanceTo(target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = target->HitPosition(_flyTime);
		_missileTimer = 0;
		_fly = true;
		_hit = false;
		MakePath();
	}
};

//----------------------------------------------//
//----------------------------------------------//
//				Замедляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

SlowMissile::SlowMissile() : FireParent() {
	_missileType = TowerType::SLOW;
	_slow = FPoint(0,0);
	_splashRange = 0;
	
};

SlowMissile::SlowMissile(MissInfo inf, std::vector<MonsterParent::Ptr> & targets) : FireParent() {
	_missileType = TowerType::SLOW;
	_position = inf._position;
	
	_modSpeed = inf._modSpeed;
	float d = _position.GetDistanceTo(inf._tPosition);
	_flyTime = d / _modSpeed;
	_targetPosition = inf._tPosition;
	
	_missileTimer = 0;
	_fly = true;
	_hit = false;
	_tex = nullptr;
	_damage = inf._damage;
	_targets = targets;
	_slow = inf._sFactor;
	_splashRange = inf._sRange;
	_missilePathX.Clear();
	_missilePathY.Clear();
	
	
	_hitEff = _misEffCont.AddEffect("RingCold");
	_hitEff->Pause();
	_hitEff->Reset();
	_hitEff->SetScale(1.3);
	
	_misEff = _misEffCont.AddEffect("IskraBlue");
	_misEff->posX = _position.x;
	_misEff->posY = _position.y;
	_misEff->Reset();
	_misEff->Pause();
	_misEff->SetScale(0.5);
	MakePath();
};

SlowMissile::~SlowMissile() {
};

void SlowMissile::DealDamage()
{
	for (unsigned int i = 0; i < _targets.size(); i++) {
		FPoint tPos = _targets[i]->Position();
		float d = sqrt((tPos.x - _position.x)*(tPos.x - _position.x) + (tPos.y - _position.y)*(tPos.y - _position.y));
		if (d < _splashRange) {
			_targets[i]->TakeDamage(_missileType, _slow, math::random(_damage.x, _damage.y));
		}
	}
}

MonsterParent::Ptr  SlowMissile::TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) {
	MonsterParent::Ptr resTarget = nullptr;
	if (target == nullptr || target->Finish() || target->Dead() || target->Dying()) {
		float d = INFINITE;
		int tarIndex = INFINITE;
		TakeNearestMonster(d, tarIndex, monsters);
		if (d <= range && tarIndex < INFINITE) {
			resTarget = monsters[tarIndex];
			_targetPosition = monsters[tarIndex]->Position();
			_targets = monsters;
			_flyTime = d / _modSpeed;
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}
	else {
		FPoint tarPos = target->Position();
		float tmpD = _position.GetDistanceTo(tarPos);

		if (tmpD <= range) {
			resTarget = target;
			_targetPosition = target->Position();
			_targets = monsters;
			_flyTime = tmpD / _modSpeed;
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}

};

//----------------------------------------------//
//----------------------------------------------//
//				Отравляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

DecayMissile::DecayMissile() : FireParent() {
	_missileType = TowerType::DECAY;
	_decay = FPoint(0,0);
	_target = nullptr;
	
};

DecayMissile::DecayMissile(MissInfo inf) : FireParent() {
	_missileType = TowerType::DECAY;
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_decay = inf._decay;
	_target = inf._target;
	_damage = inf._damage;

	_misEff = _misEffCont.AddEffect("IskraGreen");
	_misEff->posX = _position.x;
	_misEff->posY = _position.y;
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	MakePath();
	
}

DecayMissile::~DecayMissile() {
};

void DecayMissile::DealDamage()
{
	_target->TakeDamage(_missileType, _decay, math::random(_damage.x, _damage.y));
}

//----------------------------------------------//
//----------------------------------------------//
//				Оглушающий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

BashMissile::BashMissile() : FireParent() {
	_missileType = TowerType::BASH;
	_bash = FPoint(0,0);
	_target = nullptr;
};

BashMissile::BashMissile(MissInfo inf) : FireParent() {
	_missileType = TowerType::BASH;
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_bash = inf._bash;
	_target = inf._target;
	_damage = inf._damage;
	_misEff = _misEffCont.AddEffect("Iskra1");
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	MakePath();
	
};

BashMissile::~BashMissile() {
};

void BashMissile::DealDamage()
{
	_target->TakeDamage(_missileType, _bash, math::random(_damage.x, _damage.y));
}




//----------------------------------------------//
//----------------------------------------------//
//				Разрывной снаряд				//
//----------------------------------------------//
//----------------------------------------------//
SplashMissile::SplashMissile() : FireParent() {
	_missileType = TowerType::SPLASH;
	_splashRange = 0;
};

SplashMissile::SplashMissile(MissInfo inf, std::vector<MonsterParent::Ptr> & targets): FireParent() {
	_missileType = TowerType::SPLASH;
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	float d = _position.GetDistanceTo(inf._tPosition);
	_flyTime = d / _modSpeed;
	_targetPosition = inf._tPosition;
	_damage = inf._damage;
	//_targets = targets;
	_splashRange = inf._sRange;
	_hitEff = _misEffCont.AddEffect("Ring");
	_hitEff->Pause();
	_hitEff->Reset();
	_hitEff->SetScale(1.3);
		
	_misEff = _misEffCont.AddEffect("Iskra");
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	MakePath();
	
};

SplashMissile::~SplashMissile() {
};

void SplashMissile::DealDamage()
{
	for (unsigned int i = 0; i < _targets.size(); i++) {
		FPoint tPos = _targets[i]->Position();
		float d = sqrt((tPos.x - _position.x)*(tPos.x - _position.x) + (tPos.y - _position.y)*(tPos.y - _position.y));
		if (d < _splashRange) {
			_targets[i]->TakeDamage(_missileType, FPoint(0, 0), math::random(_damage.x, _damage.y));
		}

	}
}

MonsterParent::Ptr  SplashMissile::TakeAim(std::vector<MonsterParent::Ptr> & monsters, MonsterParent::Ptr target, int range) {
	MonsterParent::Ptr resTarget = nullptr;
	if (target == nullptr || target->Finish() || target->Dead() || target->Dying()) {
		float d = INFINITE;
		int tarIndex = INFINITE;
		TakeNearestMonster(d, tarIndex, monsters);
		if (d <= range && tarIndex < INFINITE) {
			resTarget = monsters[tarIndex];
			_targetPosition = monsters[tarIndex]->Position();
			_targets = monsters;
			_flyTime = d / _modSpeed;
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}
	else {
		FPoint tarPos = target->Position();
		float tmpD = _position.GetDistanceTo(tarPos);

		if (tmpD <= range) {
			resTarget = target;
			_targetPosition = target->Position();
			_targets = monsters;
			_flyTime = tmpD / _modSpeed;
			_missilePathX.Clear();
			_missilePathY.Clear();
			MakePath();
			return resTarget;
		}
		else {
			resTarget = nullptr;
			return resTarget;
		}

	}

};

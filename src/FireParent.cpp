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
	_effect.value = 0;
	_effect.length = 0;
}
FireParent::FireParent(const MissInfo & inf, const std::vector<MonsterParent::Ptr>& targets)
{
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	_damage = inf._damage;
	_target = inf._target;
	_missileTimer = 0;
	_fly = true;
	_hit = false;
	_tex = nullptr;
	_damage = inf._damage;
	
	
}
;

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
	if (!_target->Finish())
		_target->TakeDamage(_missileType, _effect, math::random(_damage.x, _damage.y));
}
IPoint FireParent::GetTargetPos(MonsterParent::Ptr monster,float dt)
{
	return monster->HitPosition(dt).Rounded();
}
;


void FireParent::SetPosition(FPoint pos) {
	_position = pos;
};

TowerType FireParent::Type() {
	return _missileType;

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
			_targetPosition = GetTargetPos(monsters[tarIndex], d / (float)_modSpeed);
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
			_targetPosition = GetTargetPos(target, tmpD / (float)_modSpeed);
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

NormalMissile::NormalMissile(const MissInfo & inf, const std::vector<MonsterParent::Ptr> & targets) : FireParent(inf, targets) {
	_missileType = TowerType::NORMAL;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_misEff = _misEffCont.AddEffect("IskraViol");
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	
};

NormalMissile::~NormalMissile() {

};



//----------------------------------------------//
//----------------------------------------------//
//				Замедляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

SlowMissile::SlowMissile() : FireParent() {
	_missileType = TowerType::SLOW;
	_splashRange = 0;
	
};

SlowMissile::SlowMissile(const MissInfo & inf, const std::vector<MonsterParent::Ptr> & targets) : FireParent(inf, targets) {
	_missileType = TowerType::SLOW;
	float d = _position.GetDistanceTo(inf._tPosition);
	_flyTime = d / _modSpeed;
	_targetPosition = inf._tPosition;
	_targets = targets;
	_effect = inf._eff;
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
				if (!_targets[i]->Finish())
					_targets[i]->TakeDamage(_missileType, _effect, math::random(_damage.x, _damage.y));
			}
		}
	
	
}

IPoint SlowMissile::GetTargetPos(MonsterParent::Ptr monster, float dt)
{
	return monster->Position().Rounded();
}



//----------------------------------------------//
//----------------------------------------------//
//				Отравляющий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

DecayMissile::DecayMissile() : FireParent() {
	_missileType = TowerType::DECAY;
	
	_target = nullptr;
	
};

DecayMissile::DecayMissile(const MissInfo & inf, const std::vector<MonsterParent::Ptr> & targets) : FireParent(inf, targets) {
	_missileType = TowerType::DECAY;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_effect = inf._eff;
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


//----------------------------------------------//
//----------------------------------------------//
//				Оглушающий снаряд				//
//----------------------------------------------//
//----------------------------------------------//

BashMissile::BashMissile() : FireParent() {
	_missileType = TowerType::BASH;
	_target = nullptr;
};

BashMissile::BashMissile(const MissInfo & inf, const std::vector<MonsterParent::Ptr> & targets) : FireParent(inf, targets) {
	_missileType = TowerType::BASH;
	if (inf._target && _modSpeed>0) {
		float d = _position.GetDistanceTo(inf._target->Position());
		_flyTime = d / _modSpeed;
		_targetPosition = inf._target->HitPosition(_flyTime);
	}
	_effect = inf._eff;
	_misEff = _misEffCont.AddEffect("Iskra1");
	_misEff->Pause();
	_misEff->Reset();
	_misEff->SetScale(0.5);
	MakePath();
	
};

BashMissile::~BashMissile() {
};






//----------------------------------------------//
//----------------------------------------------//
//				Разрывной снаряд				//
//----------------------------------------------//
//----------------------------------------------//
SplashMissile::SplashMissile() : FireParent() {
	_missileType = TowerType::SPLASH;
	_splashRange = 0;
};

SplashMissile::SplashMissile(const MissInfo & inf, const std::vector<MonsterParent::Ptr> & targets) : FireParent(inf,targets) {
	_missileType = TowerType::SPLASH;
	float d = _position.GetDistanceTo(inf._tPosition);
	_flyTime = d / _modSpeed;
	_targetPosition = inf._tPosition;
	_targets = targets;
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
				if (!_targets[i]->Finish())
					_targets[i]->TakeDamage(_missileType, _effect, math::random(_damage.x, _damage.y));
			}

		}
	
	
}

IPoint SplashMissile::GetTargetPos(MonsterParent::Ptr monster, float dt)
{
	return monster->Position().Rounded();
}


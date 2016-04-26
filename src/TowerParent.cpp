#include "stdafx.h"

#include "FireParent.h"
#include "TowerParent.h"

using namespace std;
using namespace rapidxml;
//----------------------------------------------//
//----------------------------------------------//
//			Базовый класс башни 				//
//----------------------------------------------//
//----------------------------------------------//
const IRect TowerParent::_hintTexRect = IRect(-120, -128, 240, 128);
TowerParent::TowerParent() {
	_position = FPoint(0, 0);
	_cell = IPoint(0, 0);
	_cellSize = IPoint(64,64);
	_target = nullptr;
	_reloadTime = 0;
	_reloadTimer = 0;
	_range = 0;
	_missileSpeed = 0;
	_missiles.clear();
	_texHint = Core::resourceManager.Get<Render::Texture>("Hint");
	_texCircle = Core::resourceManager.Get<Render::Texture>("Circle");
	_damage = IPoint(0, 0);
	_lvl = 0;
	_curGold = 0;
	_showUpgradeButton = false;
};


TowerParent::~TowerParent() {
}
void TowerParent::RangeDraw()
{
	if (_hint) {
		Render::BeginAlphaMul(0.15);
		_texCircle->Draw(IRect(_position.x - _range, _position.y - _range, _range * 2, _range * 2), FRect(0, 1, 0, 1));
		Render::EndAlphaMul();
	}
}
;

void TowerParent::Draw() {
	
	
	for (unsigned int i = 0; i < _missiles.size(); i++) {
		_missiles[i]->Draw();
	}

	




	if (_idleAnim && _atkAnim) {
		if (_atkAnim->IsFinished()) {
			Render::device.SetTexturing(true);

			_idleAnim->Draw(IPoint(math::round(_position.x) - _idleAnim->getFrameWidth()/2, math::round(_position.y) - _idleAnim->getFrameHeight()/3));
			
			Render::device.SetTexturing(false);
		}
		else if (!_atkAnim->IsFinished()) {
			Render::device.SetTexturing(true);
			_atkAnim->Draw(IPoint(math::round(_position.x) - _idleAnim->getFrameWidth() / 2, math::round(_position.y) - _idleAnim->getFrameHeight() / 3));
			Render::device.SetTexturing(false);
		}
	}
	
	
	Render::BindFont("dikovina_12");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(IPoint(_position.x-_cellSize.x/2,_position.y- _cellSize.y / 2), utils::lexical_cast(_lvl+1), 1.00f, LeftAlign, BottomAlign);
	Render::EndColor();
	if (_upEff) {
		_upCont.Draw();
	}
};

void TowerParent::UpgradeDraw() {

	//Подсказки

	if (_hint && _texHint) {
			IRect rect = _hintTexRect.MovedBy(_position.Rounded());
			rect.y -= _cellSize.y / 2;
		if (_position.x - rect.Width()/2 < 0) {
			rect.x = 0;
		}
		if (_position.x + rect.Width() / 2 > Render::device.Height()) {
			rect.x = Render::device.Height() - rect.Width();
		}

		if (_position.y + _hintTexRect.y < 0) {
			rect.y = _position.y + _cellSize.y / 2;
		}

		FRect uv = FRect(0, 1, 0, 1);
		Render::BeginAlphaMul(0.85);
		_texHint->Draw(rect, uv);
		Render::EndAlphaMul();

		DrawHintText(rect);

		
	}
	//Кнопка апгрейда
	if (_showUpgradeButton && _lvl < _lvlCount) {
		Render::device.SetTexturing(false);
		Render::BeginAlphaMul(0.5);
		Render::BeginColor(Color(0, 0, 0, 255));
		Render::DrawRect(_upgradeButtonRect);
		Render::EndColor();
		Render::EndAlphaMul();
		Render::device.SetTexturing(true);
		if (_upTex)
			if (_curGold < UpgradePrice()) {
				
				_upTex->Draw(_upgradeButtonRect, FRect(0, 1, 0, 1));
			}
			else {
				_upTex->Draw(_upgradeButtonRect, FRect(0, 1, 0, 1));
			}
			
		if (UpgradePrice() > 0) {


			if (_curGold < UpgradePrice()) {
				Render::BindFont("dikovina_12");
				Render::BeginColor(Color(255, 0, 0, 255));
				Render::PrintString(_upgradeButtonRect.RightBottom(), utils::lexical_cast(UpgradePrice()), 1.00f, RightAlign, BottomAlign);
				Render::EndColor();
			}
			else {
				Render::BindFont("dikovina_12");
				Render::BeginColor(Color(255, 255, 255, 255));
				Render::PrintString(_upgradeButtonRect.RightBottom(), utils::lexical_cast(UpgradePrice()), 1.00f, RightAlign, BottomAlign);
				Render::EndColor();
			}
			
		}
	}




	
};

void TowerParent::Update(float dt) {
	
	if (_atkAnim) {
		if (_idleAnim  && _atkAnim->IsFinished()) {
			_idleAnim->Update(dt);
		}
		_atkAnim->Update(dt);
	}
	if (_reloadTimer > 0)
		_reloadTimer -= dt;
	if (_reloadTimer < 0)
		_reloadTimer = 0;

	for (unsigned int i = 0; i < _missiles.size(); i++) {
		_missiles[i]->Update(dt);
	}
	for (std::vector<FireParent::Ptr>::iterator it = _missiles.begin(); it != _missiles.end();) {
		if ((*it)->Hit()) {
			it = _missiles.erase(it);
		}
		else {
			it++;
		}
			
	}
	if (_upEff) {
		_upEff->posX = _position.x;
		_upEff->posY = _position.y;
		_upCont.Update(dt);
	}
	
};

void TowerParent::UpdateAnimAngle(MonsterParent::Ptr target) {
	if (target) {



		float angle = math::atan(target->Position().y - _position.y, target->Position().x - _position.x)*180.0 / math::PI;
		if (_idleAnim && _atkAnim) {
			_idleAnim->setLoop(false);
			_idleAnim->setPlayback(false);
			if (angle > angle225.angleStart && angle <= angle225.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a315.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a315.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a315.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a315.y);
			}

			if (angle > angle270.angleStart && angle <= angle270.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a270.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a270.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a270.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a270.y);
			}
			if (angle > angle315.angleStart && angle <= angle315.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a225.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a225.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a225.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a225.y);
			}

			if (angle > angle0.angleStart && angle <= angle0.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a180.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a180.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a180.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a180.y);
			}

			if (angle > angle45.angleStart && angle <= angle45.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a135.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a135.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a135.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a135.y);
			}

			if (angle > angle90.angleStart && angle <= angle90.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a90.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a90.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a90.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a90.y);
			}

			if (angle > angle135.angleStart && angle <= angle135.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a45.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a45.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a45.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a45.y);
			}
			if (angle > angle180.angleStart || angle <= angle180.angleFinish) {
				_atkAnim->setFirstPlayedFrame(_attackAnimAngles._a0.x);
				_atkAnim->setLastPlayedFrame(_attackAnimAngles._a0.y);
				_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a0.x);
				_idleAnim->setLastPlayedFrame(_idleAnimAngles._a0.y);
			}
			_idleAnim->setCurrentFrame(0);
			_idleAnim->setLoop(true);
			_idleAnim->setPlayback(true);

			_atkAnim->setPlayback(true);

		}
	}
}


IPoint TowerParent::Cell() {
	return _cell;
};

void TowerParent::SetPosition(FPoint pos) {
	_position = pos;
	for (unsigned int i = 0; i < _missilesPrototypes.size(); i++) {
		_missilesPrototypes[i]._position = pos;
	}
	SetUButtonPosition();
	
};

void TowerParent::SetUButtonPosition() {
	IPoint buttonPos = IPoint(_position.x - 20, _position.y + 20);
	int width = 40;
	int height = 40;
	if (_position.y + height + 20 > Render::device.Height()) {
		buttonPos.y = _position.y - height - 20;
	}
	if (_position.x -20 + width > Render::device.Height()) {
		buttonPos.x = Render::device.Height() - width;
	}
	if (_position.x - 20 < 0) {
		buttonPos.x = 0;
	}
	_upgradeButtonRect = IRect(buttonPos, width, height);
	_upTex = Core::resourceManager.Get<Render::Texture>("Up");
};

void TowerParent::SetCell(IPoint cell) {
	_cell = cell;
};

void TowerParent::SetUpgradeButton(bool b) {
	_showUpgradeButton = b;
};



IRect TowerParent::UpgradeIRect() {
	return _upgradeButtonRect;
};

void TowerParent::Upgrade() {
	
	if (_lvl < _lvlCount - 1) {
		_lvl++;
		MM::manager.PlaySample("Up");
		_upEff = _upCont.AddEffect("Up");
		_upEff->SetScale(0.3);
		_upEff->posX = _position.x;
		_upEff->posY = _position.y;
		_upEff->Reset();
		
	}
}
int TowerParent::UpgradePrice()
{
	if (_lvl < _lvlCount - 1) {
		return _missilesPrototypes[_lvl + 1]._price;
	}
	else {
		return -1;
	};
}
;


void TowerParent::TakeTarget(std::vector<MonsterParent::Ptr>& monsters, FireParent::Ptr mis)
{
	MonsterParent::Ptr t;

	t = mis->TakeAim(monsters, _target, _range).get();

	_target = t;

	if (_target) {

		UpdateAnimAngle(_target);

		_missiles.push_back(mis);
		_reloadTimer = _reloadTime;
		MM::manager.PlaySample("Shoot");
	}
}

int	 TowerParent::Price() {
	return _price;
};

void  TowerParent::SetCurGold(int g) {
	_curGold = g;
};

void TowerParent::SetHint(IPoint pos) {
	IRect r = IRect(_position.x- _cellSize.x / 2, _position.y- _cellSize.y / 2, _cellSize.x, _cellSize.y);
	if (r.Contains(pos)) {
		_hint = true;
	}
	else {
		_hint = false;
	}
}

void TowerParent::LoadTowerFormXML(xml_node<>* towerNode){
	string value_t = towerNode->first_attribute("texture")->value();
	_texHint = Core::resourceManager.Get<Render::Texture>("Hint");
	_price = Xml::GetIntAttributeOrDef(towerNode, "price", 0);
	_reloadTime = Xml::GetFloatAttributeOrDef(towerNode,"reload",1);
	_reloadTimer = 0;
	_range = Xml::GetIntAttributeOrDef(towerNode, "range", 0);
	_lvlCount = Xml::GetIntAttributeOrDef(towerNode, "lvlCount", 1);

	value_t = towerNode->first_attribute("idleAnimation")->value();
	_idleAnim = Core::resourceManager.Get<Render::Animation>(value_t)->Clone();
	_idleAnimAngles = IDL_ANGLES;

	value_t = towerNode->first_attribute("atkAnimation")->value();
	_atkAnim = Core::resourceManager.Get<Render::Animation>(value_t)->Clone();
	_atkAnim->setSpeed(_atkAnim->getSpeed()*_reloadTime);
	_attackAnimAngles = ATK_ANGLES;

	for (xml_node<>* missile = towerNode->first_node("Missile"); missile; missile = missile->next_sibling("Missile")) {
		string id = missile->first_attribute("id")->value();
		FireParent::MissInfo info;
		//Общие параметры
		info._target = nullptr;
		string value;
		info._modSpeed = Xml::GetIntAttributeOrDef(missile, "misSpeed", 0);
		info._damage.x = Xml::GetIntAttributeOrDef(missile, "minDMG", 0);
		info._damage.y = Xml::GetIntAttributeOrDef(missile, "maxDMG", 0);
		info._price = Xml::GetIntAttributeOrDef(missile, "price", 0);
		
		//Эффекты атак
		//--Slow
		

		info._eff.value = Xml::GetFloatAttributeOrDef(missile, "effect", 0);
		info._eff.length = Xml::GetFloatAttributeOrDef(missile, "effectLenght", 0);


		//Радиус поражения
		info._sRange = Xml::GetFloatAttributeOrDef(missile, "splashRange", 0);
		
		
		_missilesPrototypes.push_back(info);

	}
	_price = _missilesPrototypes[0]._price;
}






//----------------------------------------------//
//----------------------------------------------//
//				Обычная башня	 				//
//----------------------------------------------//
//----------------------------------------------//

NormalTower::NormalTower() : TowerParent() {
	_towerType = TowerType::NORMAL;
};

NormalTower::NormalTower(NormalTower& proto) {
	*this = proto;
	if(proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._atkAnim)
		this->_atkAnim = proto._atkAnim->Clone();
};


NormalTower::~NormalTower() {
};

void NormalTower::TryShoot(std::vector<MonsterParent::Ptr> & monsters) {
	
	if (_reloadTimer == 0) {
		FireParent::Ptr mis = new NormalMissile(_missilesPrototypes[_lvl], monsters);
		TakeTarget(monsters, mis);

	}
	

}


void NormalTower::DrawHintText(IRect rect) {

	Render::BindFont("dikovina_10");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(FPoint(rect.x + rect.Width()/2, rect.y + 105), "%NBName%: "+ utils::lexical_cast(_lvl + 1) +"%LVL%", 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 90), "%DMG%" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.x) + "-" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.y), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 75), "%RNG%" + utils::lexical_cast(_range), 1.0f, CenterAlign, BottomAlign);
	if (_lvl<_lvlCount - 1)
		Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 30), "%UPCost%" + utils::lexical_cast(_missilesPrototypes[_lvl + 1]._price), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 15), "%DReturn%" + utils::lexical_cast(_missilesPrototypes[_lvl]._price*0.75), 1.0f, CenterAlign, BottomAlign);
	Render::EndColor();
}

//----------------------------------------------//
//----------------------------------------------//
//				Замедляющая башня	 			//
//----------------------------------------------//
//----------------------------------------------//

SlowTower::SlowTower() : TowerParent() {
	_towerType = TowerType::SLOW;
	_splashRange = 0;
	_slow = FPoint(0, 0);
};

SlowTower::SlowTower(SlowTower& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._atkAnim)
		this->_atkAnim = proto._atkAnim->Clone();
};


SlowTower::~SlowTower() {};

void SlowTower::TryShoot(std::vector<MonsterParent::Ptr> & monsters) {

	if (_reloadTimer == 0) {
		FireParent::Ptr mis = new SlowMissile(_missilesPrototypes[_lvl], monsters);
		TakeTarget(monsters, mis);

	}


}



void SlowTower::DrawHintText(IRect rect) {

	Render::BindFont("dikovina_10");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 105), "%SLBName% : " + utils::lexical_cast(_lvl + 1) + "%LVL%", 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 90), "%DMG%" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.x) + "-" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.y), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 75), "%RNG%" + utils::lexical_cast(_range), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 60), "%SPLASH%" + utils::lexical_cast(_missilesPrototypes[_lvl]._sRange), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 45), "%SLOW%" + utils::lexical_cast((int)(_missilesPrototypes[_lvl]._eff.value * 100))+"%ON%"+ utils::lexical_cast((int)_missilesPrototypes[_lvl]._eff.length)+"%SEC%", 1.0f, CenterAlign, BottomAlign);

	
	if (_lvl<_lvlCount - 1)
		Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 30), "%UPCost%" + utils::lexical_cast(_missilesPrototypes[_lvl + 1]._price), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 15), "%DReturn%" + utils::lexical_cast(_missilesPrototypes[_lvl]._price*0.75), 1.0f, CenterAlign, BottomAlign);
	Render::EndColor();
}

//----------------------------------------------//
//----------------------------------------------//
//				Отравляющая башня	 			//
//----------------------------------------------//
//----------------------------------------------//

DecayTower::DecayTower() : TowerParent() {
	_towerType = TowerType::DECAY;
	_decay = FPoint(0, 0);
};

DecayTower::DecayTower(DecayTower& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._atkAnim)
		this->_atkAnim = proto._atkAnim->Clone();
};


DecayTower::~DecayTower() {};

void DecayTower::TryShoot(std::vector<MonsterParent::Ptr> & monsters) {

	if (_reloadTimer == 0) {
		FireParent::Ptr mis = new DecayMissile(_missilesPrototypes[_lvl], monsters);
		TakeTarget(monsters, mis);

	}


}


void DecayTower::DrawHintText(IRect rect) {

	Render::BindFont("dikovina_10");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 105), "%PBName% :" + utils::lexical_cast(_lvl + 1) + " lvl", 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 90), "%DMG%" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.x) + "-" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.y), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 75), "%RNG%" + utils::lexical_cast(_range), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 60), "%POISON%" + utils::lexical_cast((int)_missilesPrototypes[_lvl]._eff.value) + "%DPS%" + utils::lexical_cast((int)_missilesPrototypes[_lvl]._eff.length) + "%SEC%", 1.0f, CenterAlign, BottomAlign);


	if (_lvl<_lvlCount - 1)
		Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 30), "Upgrade cost : " + utils::lexical_cast(_missilesPrototypes[_lvl + 1]._price), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 15), "Destroy returns : " + utils::lexical_cast(_missilesPrototypes[_lvl]._price*0.75), 1.0f, CenterAlign, BottomAlign);
	Render::EndColor();
}


//----------------------------------------------//
//----------------------------------------------//
//				Оглушающая башня	 			//
//----------------------------------------------//
//----------------------------------------------//
BashTower::BashTower() : TowerParent() {
	_towerType = TowerType::BASH;
	_bash = FPoint(0, 0);
};

BashTower::BashTower(BashTower& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._atkAnim)
		this->_atkAnim = proto._atkAnim->Clone();
};

BashTower::~BashTower() {};

void BashTower::TryShoot(std::vector<MonsterParent::Ptr> & monsters) {

	if (_reloadTimer == 0) {
		FireParent::Ptr mis = new BashMissile(_missilesPrototypes[_lvl], monsters);
		TakeTarget(monsters, mis);

	}


}


void BashTower::DrawHintText(IRect rect) {

	Render::BindFont("dikovina_10");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 105), "%BBName% :" + utils::lexical_cast(_lvl + 1) + "%LVL%", 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 90), "%DMG%" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.x) + "-" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.y), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 75), "%RNG%" + utils::lexical_cast(_range), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 60), "%BASH%"+ utils::lexical_cast((int)(_missilesPrototypes[_lvl]._eff.value*100)) + "%ON%" + utils::lexical_cast((int)_missilesPrototypes[_lvl]._eff.length) + "%SEC%", 1.0f, CenterAlign, BottomAlign);
	

	if (_lvl<_lvlCount - 1)
		Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 30), "%UPCost%" + utils::lexical_cast(_missilesPrototypes[_lvl + 1]._price), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 15), "%DReturn%" + utils::lexical_cast(_missilesPrototypes[_lvl]._price*0.75), 1.0f, CenterAlign, BottomAlign);
	Render::EndColor();
}


//----------------------------------------------//
//----------------------------------------------//
//				Разрывная башня	 				//
//----------------------------------------------//
//----------------------------------------------//

SplashTower::SplashTower() : TowerParent() {
	_towerType = TowerType::SPLASH;
	_splashRange = 0;
};

SplashTower::SplashTower(SplashTower& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._atkAnim)
		this->_atkAnim = proto._atkAnim->Clone();
};

SplashTower::~SplashTower() {};

void SplashTower::TryShoot(std::vector<MonsterParent::Ptr> & monsters) {

	if (_reloadTimer == 0) {
		FireParent::Ptr mis = new SplashMissile(_missilesPrototypes[_lvl],monsters);
		TakeTarget(monsters, mis);

	}


}



void SplashTower::DrawHintText(IRect rect) {

	Render::BindFont("dikovina_10");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 105), "%SPBName%: " + utils::lexical_cast(_lvl+1) + "%LVL%", 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 90), "%DMG%" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.x) + "-" + utils::lexical_cast(_missilesPrototypes[_lvl]._damage.y), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 75), "%RNG%" + utils::lexical_cast(_range), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 60), "%SPLASH%" + utils::lexical_cast(_missilesPrototypes[_lvl]._sRange), 1.0f, CenterAlign, BottomAlign);
	if (_lvl<_lvlCount - 1)
		Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 30), "%UPCost%" + utils::lexical_cast(_missilesPrototypes[_lvl + 1]._price), 1.0f, CenterAlign, BottomAlign);
	Render::PrintString(FPoint(rect.x + rect.Width() / 2, rect.y + 15), "%DReturn%" + utils::lexical_cast(_missilesPrototypes[_lvl]._price*0.75), 1.0f, CenterAlign, BottomAlign);
	Render::EndColor();
}




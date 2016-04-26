#include "stdafx.h"
#include "FireParent.h"
#include "MonsterParent.h"
const short FINISH_MARK = 9999;
const short PASS_MARK = 9998;
const short NOT_PASS_MARK = -1;
const IPoint MonsterParent::_meatTexOffset = IPoint(56,71);
const IPoint MonsterParent::_meatTexSize = IPoint(15, 15);

//----------------------------------------------//
//----------------------------------------------//
//			Базовый класс монстра 				//
//----------------------------------------------//
//----------------------------------------------//

MonsterParent::MonsterParent() {
	_position = FPoint(0, 0);
	_modSpeed = 0;
	_map = nullptr;
	_curWayDistance = 0;
	_moveTimer = 0;
	_hp = 0;
	_maxHp = _hp;
	_empty = false;
	_dead = false;
	_dying = false;
	_finish = false;
	_runAnimAngles = RUN_ANGLES;
	_idleAnimAngles = MOB_IDL_ANGLES;
	_dieAnimAngles = DIE_ANGLES;
	_runAnim = nullptr;
	_idleAnim = nullptr;
	_dieAnim = nullptr;
	_lastAngle = 20;
	_damage = 1;
	_meat = nullptr;
	_meatCont = 0;
	_dieSound = "Die";
	_meatFlyTimer = 0;
	_dieAnimEnd = false;
	_curWaySplineX.Clear();
	_curWaySplineY.Clear();
}
MonsterParent::MonsterParent(const MonsterInfo & inf)
{
	
	_dieAnimEnd = false;
	_lastAngle = 20;
	_curWayDistance = 0;
	_moveTimer = 0;
	_meatFlyTimer = 0;
	_empty = false;
	_dead = false;
	_dying = false;
	_finish = false;
	_runAnimAngles = RUN_ANGLES;
	_idleAnimAngles = MOB_IDL_ANGLES;
	_dieAnimAngles = DIE_ANGLES;
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	_map = inf._map;
	_hp = inf._hp;
	_maxHp = _hp;
	_runAnim = inf._runAnim;
	_idleAnim = inf._idleAnim;
	_dieAnim = inf._dieAnim;
	_meat = Core::resourceManager.Get<Render::Texture>("Meat");
	_dieSound = inf._dieSound;
	_meatCont = inf._meat;
	_curWaySplineX.Clear();
	_curWaySplineY.Clear();
	_bash.value = 0;
	_decay.value =0;
	_slow.value = 0;
}
;

MonsterParent::~MonsterParent() {};

void MonsterParent::Draw() {
	if (!_dead && !_finish) {
		
		if (_idleAnim && _runAnim && _dieAnim) {
			
			Render::BeginColor(_effectColor);
			IPoint pos = IPoint(_position.x - _map->CellSize().x, _position.y - _map->CellSize().y * 2 / 3);
			Render::device.SetTexturing(true);
			if (!_dying) {
				
				if (_bash.length > 0 ) {
					_idleAnim->Draw(pos);
				}
				else {
					_runAnim->Draw(pos);
				}
			}
			else {
				_dieAnim->Draw(pos);

			}
			
			Render::EndColor();

			Render::device.SetTexturing(false);
			int width = 30 * _hp / _maxHp;
			if (width < 0)
				width = 0;
			IRect cRect = IRect(_position.x - 15, _position.y - 15, width, 5);
			Render::BeginColor(Color(0, 255, 0, 255));
			Render::DrawRect(cRect);
			Render::EndColor();

			
		}
	}
	_Cont.Draw();
};

void MonsterParent::DrawMeat() {
	if (_dying && !_dead) {

		//if (_meatEff)
		//	_meatEff->Draw();
		IPoint pos = IPoint(_position.x - _map->CellSize().x, _position.y - _map->CellSize().y * 2 / 3);
		Render::BeginColor(Color(255, 255, 255, 255 - 255 * _meatFlyTimer/2));
		if (_meatFlyTimer > 0 && _meatFlyTimer <= 0.50) {
			_meat->Draw(FRect(pos.x + _meatTexOffset.x - 10 * _meatFlyTimer,
				pos.x + _meatTexOffset.x + _meatTexSize.x + 10 * _meatFlyTimer,
				pos.y + _meatTexOffset.y + 10 * _meatFlyTimer,
				pos.y + _meatTexOffset.y + _meatTexSize.y + 20 * _meatFlyTimer),
				FRect(0, 1, 0, 1));

			Render::BindFont("dikovina_12");

			Render::PrintString(FPoint(pos.x + _meatTexOffset.x + _meatTexSize.x + 10 * _meatFlyTimer, pos.y + _meatTexOffset.y + 10 * _meatFlyTimer), "+" + utils::lexical_cast(_meatCont), 1.00f + _meatFlyTimer, LeftAlign, BottomAlign);
		}
		
		Render::EndColor();
	}
	
};

void MonsterParent::Update(float dt) {
	//Таймер замедления
	if (_slow.length <= 0) {
		_slow.value = 0;
		_slow.length = 0;
	}
	else {
		_slow.length -= dt;
	}
	//Таймер отравления
	if (_decay.length <= 0) {
		_decay.value = 0;
		_decay.length = 0;
	}
	else {
		_decay.length -= dt;
	}
	//Таймер оглушения
	if (_bash.length <= 0) {
		_bash.value = 0;
		_bash.length = 0;
	}
	else {
		_bash.length -= dt;
	}

	_hp -= _decay.value * dt;

	if (_hp <= 0 && !_dying) {
		_dying = true;	
		MM::manager.PlaySample(_dieSound);
		_meatEff = _Cont.AddEffect("MeatEff");
		_meatEff->posX = _position.x;
		_meatEff->posY = _position.y;
		_meatEff->Reset();
	}
	
	PostUpdate(dt);

	if(_dieAnim->IsFinished() && _meatFlyTimer > 0.5)
		_dead = true;
	
	
	if (_dying && !_dieAnim->IsPlaying() && !_dieAnimEnd) {
		_dieAnim->setPlayback(true);
		_dieAnimEnd = true;	
	}
	float edt = dt;
	edt *= (1 - _slow.value)*(1 - _bash.value);

	_moveTimer += edt;
	_curWayDistance -= _modSpeed*edt;
	if (_curWayDistance <= 0 && !_dying)
		_finish = true;
	if (!_dying) {
		_position.x = _curWaySplineX.getGlobalFrame(_moveTimer);
		_position.y = _curWaySplineY.getGlobalFrame(_moveTimer);
	}
	
	UpdateAnimAngle(edt);
	if (_idleAnim && _runAnim && _dieAnim) {
		

		if (!_dying) {
			
			if (_bash.length > 0) {
				_idleAnim->Update(dt);
			}
			else {
				_runAnim->Update(edt*0.4);
			}
			
			
		}
		else {
			_meatFlyTimer += dt;
			_dieAnim->Update(dt);
		}
		
		

	}

	_Cont.Update(dt);
	if (_coldEff && _runAnim) {
		_coldEff->posX = _position.x;
		_coldEff->posY = _position.y + _runAnim->getFrameHeight() / 4;
	}
	if (_stunEff && _idleAnim) {
		_stunEff->posX = _position.x;
		_stunEff->posY = _position.y + _runAnim->getFrameHeight() / 3;
	}
	if (_poisonEff && _runAnim) {
		_poisonEff->posX = _position.x;
		_poisonEff->posY = _position.y + _runAnim->getFrameHeight() / 4;
	}

	//Включение эффектов
	_effectColor = Color(255, 255, 255, 255);
	if (_slow.value > 0 && _decay.value> 0) {
		_effectColor = Color(0, 250, 154, 255);
		
		AddEffect("Cold");
		AddEffect("Poison");
	}else if (_slow.value > 0) {
		_effectColor = Color(60, 124, 255, 255);
		AddEffect("Cold");
	}
	else if (_decay.value>0) {
		_effectColor = Color(124, 252, 0, 255);
		AddEffect("Poison");
	}

	if (_bash.value > 0) {
		AddEffect("Stun");
	}


	//Выключение эффектов
	if (_slow.value == 0 && _coldEff) {
		_coldEff->Finish();
	}
	if (_bash.value == 0 && _stunEff) {
		_stunEff->Finish();
	}
	if (_decay.value == 0 && _poisonEff) {
		_poisonEff->Finish();
	}

};

void MonsterParent::UpdateAnimAngle(float dt) {
		
	float x = _curWaySplineX.getGlobalFrame(_moveTimer);
	float y = _curWaySplineY.getGlobalFrame(_moveTimer);
	float x1 = _curWaySplineX.getGlobalFrame(_moveTimer + dt);
	float y1 = _curWaySplineY.getGlobalFrame(_moveTimer + dt);
		float angle = math::atan(y1 - y, x1 - x )*180.0 / math::PI;
		if (dt == 0)
			angle = _lastAngle;
		if (_idleAnim && _runAnim && _dieAnim) {
			
			
			if (angle > angle225.angleStart && angle <= angle225.angleFinish) {

				if (_lastAngle != -135) {
					
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);

					_runAnim->setFirstPlayedFrame(_runAnimAngles._a315.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a315.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a315.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a315.y);

					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a315.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a315.y);
					_dieAnim->setCurrentFrame(0);
					
					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);


					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
					

				}
				
				_lastAngle = -135;
			}
			

			if (angle > angle270.angleStart && angle <= angle270.angleFinish) {
				if (_lastAngle != -90) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);
					_runAnim->setFirstPlayedFrame(_runAnimAngles._a270.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a270.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a270.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a270.y);

					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a270.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a270.y);
					_dieAnim->setCurrentFrame(0);
					

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = -90;
			}
			if (angle > angle315.angleStart && angle <= angle315.angleFinish) {
				if (_lastAngle != -45) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);
					_runAnim->setFirstPlayedFrame(_runAnimAngles._a225.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a225.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a225.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a225.y);
			
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a225.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a225.y);
					_dieAnim->setCurrentFrame(0);
					

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = -45;
			}

			if (angle > angle0.angleStart && angle <= angle0.angleFinish) {
				if (_lastAngle != 0) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);
					_runAnim->setFirstPlayedFrame(_runAnimAngles._a180.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a180.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a180.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a180.y);
				
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a180.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a180.y);
					_dieAnim->setCurrentFrame(0);
					
					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 0;
			}

			if (angle > angle45.angleStart && angle <= angle45.angleFinish) {
				if (_lastAngle != 45) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);
					_runAnim->setFirstPlayedFrame(_runAnimAngles._a135.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a135.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a135.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a135.y);
			
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a135.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a135.y);
					_dieAnim->setCurrentFrame(0);
					

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 45;
			}

			if (angle > angle90.angleStart && angle <= angle90.angleFinish) {
				if (_lastAngle != 90) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);

					_runAnim->setFirstPlayedFrame(_runAnimAngles._a90.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a90.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a90.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a90.y);
				
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a90.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a90.y);
					_dieAnim->setCurrentFrame(0);
					

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 90;
			}

			if (angle > angle135.angleStart && angle <= angle135.angleFinish) {
				if (_lastAngle != 135) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);

					_runAnim->setFirstPlayedFrame(_runAnimAngles._a45.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a45.y);

					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a45.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a45.y);
					
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a45.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a45.y);
					_dieAnim->setCurrentFrame(0);
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 135;
			}
			if (angle > angle180.angleStart || angle <= angle180.angleFinish) {
				if (_lastAngle != 180) {
					_runAnim->setLoop(false);
					_runAnim->setPlayback(false);

					_idleAnim->setLoop(false);
					_idleAnim->setPlayback(false);

					_runAnim->setFirstPlayedFrame(_runAnimAngles._a0.x);
					_runAnim->setLastPlayedFrame(_runAnimAngles._a0.y);
				
					_idleAnim->setFirstPlayedFrame(_idleAnimAngles._a0.x);
					_idleAnim->setLastPlayedFrame(_idleAnimAngles._a0.y);
			
					
					_dieAnim->setFirstPlayedFrame(_dieAnimAngles._a0.x);
					_dieAnim->setLastPlayedFrame(_dieAnimAngles._a0.y);
					_dieAnim->setCurrentFrame(0);
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 180;
			}
			

			

		}
	
}


bool MonsterParent::FindAWay() {

	//Разметка карты проходимости
	std::vector<std::vector<int>> intMap;
	intMap.resize(_map->Size().x);
	for ( unsigned int i = 0; i < intMap.size(); i++) {
		intMap[i].resize(_map->Size().y);
	}
	for (unsigned int i = 0; i < intMap.size(); i++) {
		for (unsigned int j = 0; j < intMap.size(); j++) {
			intMap[i][j] = -1;
			switch (_map->Cells()[i][j]->Type())
			{
			case PASS:
				intMap[i][j] = PASS_MARK;
				break;
			case SPAWN:
				intMap[i][j] = PASS_MARK;
				break;
			case LAIR:
				intMap[i][j] = FINISH_MARK;
				break;
			default:
				intMap[i][j] = NOT_PASS_MARK;
				break;
			}
		}
	}


	//Разметка прохождения волны
	//_curCell = IPoint(0, 5);
	//IPoint finish = _curCell;
	_currentWay.clear();
	intMap[_curCell.x][_curCell.y] = 0;
	int d = 0;
	bool end = false;
	std::vector<IPoint> wave;
	wave.push_back(_curCell);
	while (!end && d <= intMap.size() * intMap[0].size()) {//Пока не достигнут пункт назначения
		wave = FillAround(wave, intMap, d);
		if (_currentWay.size() > 0)
			end = true;
		++d;// Повышаем значение волны
	}


	bool recover = false;
	//Восстановление пути
	if (_currentWay.size() > 0) {//Если найден путь к пункту назначения
		
		while (d >= 0) {//Пока значение волны больше нуля
			//Для всех клеток
			for (int k = 0; k < intMap.size(); k++) {
				for (int l = 0; l < intMap[k].size(); l++) {
				
					if (intMap[k][l] == d - 1) {//если значение их волны меньше на 1
								//и они находятся в окрестности текущей клетки
						if (d > 0 && math::abs(_currentWay[_currentWay.size() - 1].x - k) <= 1 && math::abs(_currentWay[_currentWay.size() - 1].y - l <= 1)) {
							_currentWay.push_back(IPoint(k, l));//добавляем клетку в путь 
							recover = true;
							break;//и прекращаем поиск... 
						}
									
								
							}
							
				}

				if (recover)
					break; //...для текущего значения волны
			}
			recover = false;
			--d;//Понижаем значение волны
		}
		
	} 

	for (unsigned int i = 0; i < intMap.size(); i++) {
		for (unsigned int j = 0; j < intMap[i].size(); j++) {
			if (intMap[i][j]!= NOT_PASS_MARK && intMap[i][j]!=FINISH_MARK) {
				intMap[i][j] = PASS_MARK;
			}
		}
	}


	
	std::reverse(_currentWay.begin(), _currentWay.end());
	if (_currentWay.size() > 0) {
		return true;
	}
	else {
		return false;
	}
		

};

std::vector<IPoint> MonsterParent::FillAround(std::vector<IPoint> lastWaveFilled, std::vector<std::vector<int>> & map, int d) {
	std::vector<IPoint> result;
	bool end = false;
	for (unsigned int cell = 0; cell < lastWaveFilled.size(); cell++) {
		int i = lastWaveFilled[cell].x;
		int j = lastWaveFilled[cell].y;
		for (int k = i - 1; k <= i + 1; k++) {
			for (int l = j - 1; l <= j + 1; l++) {
				//не выходя за границы карты.
				if (k >= 0 && k < map.size()) {
					if (l >= 0 && l < map[k].size()) {
						//Если смежная клетка не пункт назначения
						if (map[k][l] != FINISH_MARK) {
							//и является проходимой и не помеченной волной
							if (map[k][l] == PASS_MARK) {
								map[k][l] = d + 1;//метим ее значением следующей волны.
								result.push_back(IPoint(k, l));
							}
						}
						else {
							end = true;
							_currentWay.push_back(IPoint(k, l));//Достигнут конец, добавляем в путь конечную точку 
							break;//и прекращаем поиск пункта назначения... 
						}
					}
				}

			}

			if (end)
				break;//...среди смежных точек
		}
	}
	return result;
};

bool MonsterParent::Dead() {
	return _dead;
};

bool MonsterParent::Dying() {
	return _dying;
};

bool MonsterParent::EndDeadAnim() {
	if (_dieAnim) {
		return _dieAnim->IsFinished();
	}
	else {
		return true;
	}
};

bool MonsterParent::Finish() {
	return _finish;
};

float MonsterParent::WayDistance() {
	return _curWayDistance;
};

FPoint MonsterParent::Position() {
	return _position;
};

FPoint MonsterParent::HitPosition(float dt) {
	float edt = dt;
	edt *=(1 - _slow.value)*(1 - _bash.value);
	return FPoint(_curWaySplineX.getGlobalFrame(edt + _moveTimer),
				  _curWaySplineY.getGlobalFrame(edt + _moveTimer));
};

void MonsterParent::SetPosition(FPoint pos,FieldMap *map) {
	_position = pos;
	_map = map;
	_curCell = _map->PosCell(_position);
	_curWayDistance = 0;
	float timer = 0;
	if (FindAWay()) {
		_curWaySplineX.addKey(timer, _position.x);
		_curWaySplineY.addKey(timer, _position.y);
		for (unsigned int i = 1; i < _currentWay.size(); i++) {
			FPoint newCell = FPoint(_currentWay[i].x * _map->CellSize().x + _map->CellSize().x / 2, _currentWay[i].y * _map->CellSize().y + _map->CellSize().y / 2);
			FPoint oldCell = FPoint(_currentWay[i - 1].x * _map->CellSize().x + _map->CellSize().x / 2, _currentWay[i - 1].y * _map->CellSize().y + _map->CellSize().y / 2);
			float distance = sqrt((newCell.x - oldCell.x)*(newCell.x - oldCell.x) + (newCell.y - oldCell.y)*(newCell.y - oldCell.y));
			_curWayDistance += distance;
			timer += distance / _modSpeed;
			_curWaySplineX.addKey(timer, newCell.x);
			_curWaySplineY.addKey(timer, newCell.y);

		}
		_curWaySplineX.CalculateGradient();
		_curWaySplineY.CalculateGradient();
	}
	else {
		_curWaySplineX.Clear();
		_curWaySplineY.Clear();
	}



	_moveTimer = 0;
};

int MonsterParent::TakeMonsterMeat() {
	if (!_empty) {
		_empty = true;
		return _meatCont;
	}else{
		return 0;
	}
}
void MonsterParent::AddEffect(std::string eff)
{
	if (eff == "Poison") {
		if (!_Cont.IsEffectAlive(_poisonEff.get())) {
			_poisonEff = _Cont.AddEffect("Poison");
			_poisonEff->Reset();

		}
	}
	else if (eff == "Stun") {
		if (!_Cont.IsEffectAlive(_stunEff.get())) {
			_stunEff = _Cont.AddEffect("Stun");
			_stunEff->Reset();

		}
	}
	else if (eff == "Cold") {
		if (!_Cont.IsEffectAlive(_coldEff.get())) {
			_coldEff = _Cont.AddEffect("Cold");
			_coldEff->Reset();

		}
	}
}
bool MonsterParent::isEmpty()
{
	return _empty;
}
IPoint MonsterParent::GetPoisition()
{
	return _position.Rounded();
}
;


//----------------------------------------------//
//----------------------------------------------//
//				Обычный монстр    				//
//----------------------------------------------//
//----------------------------------------------//

NormalMonster::NormalMonster() : MonsterParent() {
	
};

NormalMonster::NormalMonster(NormalMonster& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._runAnim)
		this->_runAnim = proto._runAnim->Clone();
	if (proto._dieAnim)
		this->_dieAnim = proto._dieAnim->Clone();
	if (proto._coldEff)
		this->_coldEff = proto._coldEff->Clone();
	if (proto._stunEff)
		this->_stunEff = proto._stunEff->Clone();
	if (proto._poisonEff)
		this->_poisonEff = proto._poisonEff->Clone();
};

NormalMonster::NormalMonster(const MonsterInfo& inf) : MonsterParent(inf) {
	_damage = 1;
};

NormalMonster::~NormalMonster() {
};

void NormalMonster::TakeDamage(TowerType effType, AttackEffect values, float damage) {
	if (effType == TowerType::SLOW)
		_slow = values;
	if (effType == TowerType::DECAY)
		_decay = values;
	if (effType == TowerType::BASH) {
		values.value *= 100;
		if (math::random(0, 100) <= values.value)
			_bash.value = 1;
			_bash.length = values.length;
	}

	if (_hp > 0) {
		_hp -= damage;
	}
	
};


//----------------------------------------------//
//----------------------------------------------//
//						Босс    				//
//----------------------------------------------//
//----------------------------------------------//

BossMonster::BossMonster() : MonsterParent(){
	_reduceDamage = 0;
};

BossMonster::BossMonster(BossMonster& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._runAnim)
		this->_runAnim = proto._runAnim->Clone();
	if (proto._dieAnim)
		this->_dieAnim = proto._dieAnim->Clone();
	if (proto._coldEff)
		this->_coldEff = proto._coldEff->Clone();
	if (proto._stunEff)
		this->_stunEff = proto._stunEff->Clone();
	if (proto._poisonEff)
		this->_poisonEff = proto._poisonEff->Clone();
};

BossMonster::BossMonster(const MonsterInfo& inf) : MonsterParent(inf) {
	_reduceDamage = inf._reduceDamage;
	_damage = 5;
};

BossMonster::~BossMonster() {
};

void BossMonster::TakeDamage(TowerType effType, AttackEffect values, float damage) {
	
	
	
	if (effType == TowerType::SLOW)
		_slow = values;
	if (effType == TowerType::DECAY) {
		_decay.value = 4 * values.value;
		_decay.length = 4 * values.length;
	}
	if (_hp > 0) {
		_hp -= damage * (1 - _reduceDamage);
	}

};

//----------------------------------------------//
//----------------------------------------------//
//					Имунный		   				//
//----------------------------------------------//
//----------------------------------------------//
ImmuneMonster::ImmuneMonster() : MonsterParent() {
	
};

ImmuneMonster::ImmuneMonster(ImmuneMonster& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._runAnim)
		this->_runAnim = proto._runAnim->Clone();
	if (proto._dieAnim)
		this->_dieAnim = proto._dieAnim->Clone();
	if (proto._coldEff)
		this->_coldEff = proto._coldEff->Clone();
	if (proto._stunEff)
		this->_stunEff = proto._stunEff->Clone();
	if (proto._poisonEff)
		this->_poisonEff = proto._poisonEff->Clone();
};

ImmuneMonster::ImmuneMonster(const MonsterInfo& inf) : MonsterParent(inf) {
	_damage = 1;
};

ImmuneMonster::~ImmuneMonster() {
};

void ImmuneMonster::TakeDamage(TowerType effType, AttackEffect values, float damage) {
	if (effType == TowerType::DECAY)
		_decay = values;
	if (effType == TowerType::BASH) {
		values.value *= 100;
		if (math::random(0, 100) <= values.value) {
			_bash.value = 1;
			_bash.length = values.length;
			damage *= 2;
		}
			
	}

	if (_hp > 0) {
		_hp -= damage;
	}

};

//----------------------------------------------//
//----------------------------------------------//
//				Регенерирующий    				//
//----------------------------------------------//
//----------------------------------------------//

HealingMonster::HealingMonster() : MonsterParent() {
	_healPerSecond = 0;
};

HealingMonster::HealingMonster(HealingMonster& proto) {
	*this = proto;
	if (proto._idleAnim)
		this->_idleAnim = proto._idleAnim->Clone();
	if (proto._runAnim)
		this->_runAnim = proto._runAnim->Clone();
	if (proto._dieAnim)
		this->_dieAnim = proto._dieAnim->Clone();
	if (proto._coldEff)
		this->_coldEff = proto._coldEff->Clone();
	if (proto._stunEff)
		this->_stunEff = proto._stunEff->Clone();
	if (proto._poisonEff)
		this->_poisonEff = proto._poisonEff->Clone();
};

HealingMonster::HealingMonster(const MonsterInfo& inf) : MonsterParent(inf) {
	_healPerSecond = inf._healPerSecond;
	_damage = 1;
	
};

HealingMonster::~HealingMonster() {
};

void HealingMonster::TakeDamage(TowerType effType, AttackEffect values, float damage) {
	if (effType == TowerType::SLOW) {
		_slow.value = 2 * values.value;
		_slow.length = values.length*1.5;
		if (_slow.value >= 1) {
			_slow.value = 0.8;
		}
	}
		
	
	if (effType == TowerType::BASH) {
		values.value *= 100;
		if (math::random(0, 100) <= values.value) {
			_bash.value = 1;
			_bash.length = values.length;
		}

	}

	if (_hp > 0) {
		_hp -= damage;
		
	}
	
	

}

void HealingMonster::PostUpdate(float dt) {
	if (!_dying && _hp < _maxHp) {
		_hp = _hp + _healPerSecond*dt;
		if (_hp > _maxHp)
			_hp = _maxHp;
	}
	

};
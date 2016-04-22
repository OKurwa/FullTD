#include "stdafx.h"
#include "FireParent.h"
#include "MonsterParent.h"
const short YOU_PASSED = 9999;
const short YOU_SHALL_PASS = 9998;
const short YOU_SHALL_NOT_PASS = -1;
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
	_damaged = false;
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
	_curWaySplineX.Clear();
	_curWaySplineY.Clear();
};

MonsterParent::MonsterParent(FPoint position, int modSpeed, int hp, FieldMap * map, Render::TexturePtr skin) {
	_position = position;
	_modSpeed = modSpeed;
	_map = map;
	_curCell = map->PosCell(_position);
	_curWayDistance = 0;
	float timer = 0;
	if (FindAWay()) {
		_curWaySplineX.addKey(timer, _position.x);
		_curWaySplineY.addKey(timer, _position.y);
		for (int i = 1; i < _currentWay.size(); i++) {
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
	
	
	_hp = hp;
	_maxHp = _hp;
	_damaged = false;
	
	_skin = skin;
	_dead = false;
	_finish = false;
};

MonsterParent::~MonsterParent() {};

void MonsterParent::Draw() {
	if (!_dead && !_finish) {


		
		if (_idleAnim && _runAnim && _dieAnim) {
			IPoint pos = IPoint(_position.x - 64, _position.y - 40);

			if (_idleAnim && _runAnim && _dieAnim) {
				Render::device.SetTexturing(true);
				//Render::device.PushMatrix();
				//Render::device.MatrixTranslate(pos);
				//Render::device.MatrixScale(0.5, 0.5, 1);
				//Render::device.MatrixTranslate(pos.x*2,pos.y*2,0);
				if (!_dying) {

					if (_bash != FPoint(0, 0)) {
						_idleAnim->Draw(pos);
					}
					else {
						_runAnim->Draw(pos);
					}


			}
			else {
				_dieAnim->Draw(pos);
				
			}

			//Render::device.PopMatrix();

			Render::device.SetTexturing(false);
			int width = 30 * _hp / _maxHp;
			if (width < 0)
				width = 0;
			IRect cRect = IRect(_position.x - 15, _position.y - 15, width, 5);
			//Render::device.SetTexturing(false);
			Render::BindFont("arial");

			//Render::BeginColor(Color(255 - 255*_hp / _maxHp, 255* _hp / _maxHp, 0, 255));
			Render::BeginColor(Color(0, 255, 0, 255));
			Render::DrawRect(cRect);
			Render::EndColor();


		}
		
		/*
		if (_idleAnim && _runAnim && _dieAnim) {
			IPoint pos = IPoint(_position.x - 32, _position.y - 20);

			if (_idleAnim && _runAnim && _dieAnim) {
				Render::device.SetTexturing(true);
				Render::device.PushMatrix();
				//Render::device.MatrixTranslate(pos);
				Render::device.MatrixScale(0.5, 0.5, 1);
				Render::device.MatrixTranslate(pos.x*2,pos.y*2,0);
				if (!_dying) {

					if (_bash != FPoint(0, 0)) {
						_idleAnim->Draw();
					}
					else {
						_runAnim->Draw();
					}
					

				}
				else {
					_dieAnim->Draw();
					_meat->Draw(FRect(46, 76,66, 96), FRect(0, 1, 0, 1));
					Render::BindFont("arial");
					Render::BeginColor(Color(255, 255, 255, 255));
					Render::PrintString(FPoint(76, 66), "+", 2.00f, LeftAlign, BottomAlign);
					Render::EndColor();
					//_meat->DrawRect(FRect(pos.x, pos.x + 64, pos.y + 10, pos.y + 74), FRect(0, 1, 0, 1));
				}
				
				Render::device.PopMatrix();
				
				Render::device.SetTexturing(false);
				int width = 30 * _hp / _maxHp;
				if (width < 0)
					width = 0;
				IRect cRect = IRect(_position.x - 15, _position.y - 15, width, 5);
				//Render::device.SetTexturing(false);
				Render::BindFont("arial");

				//Render::BeginColor(Color(255 - 255*_hp / _maxHp, 255* _hp / _maxHp, 0, 255));
				Render::BeginColor(Color(0, 255, 0, 255));
				Render::DrawRect(cRect);
				Render::EndColor();
				
				
			}
			*/
		}
		else {
			IRect cRect = IRect(_position.x - 2, _position.y - 2, 5, 5);
			//Render::device.SetTexturing(false);
			Render::BindFont("arial");

			Render::BeginColor(Color(100, 200, 100, 255));
			Render::DrawRect(cRect);
			Render::EndColor();
			Render::device.SetTexturing(true);
			Render::BeginColor(Color(255, 255, 255, 255));
			Render::PrintString(FPoint(_position.x - 2, _position.y - 2), utils::lexical_cast(math::round(_hp)), 0.70f);
			Render::EndColor();
			Render::device.SetTexturing(false);
			//Render::device.SetTexturing(true);
		}


		
	}
};

void MonsterParent::DrawMeat() {
	if (_dying && !_dead) {
		IPoint pos = IPoint(_position.x - 64, _position.y - 40);
		_meat->Draw(FRect(pos.x + 56, pos.x + 71, pos.y + 86, pos.y + 101), FRect(0, 1, 0, 1));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(pos.x + 71, pos.y + 86), "+" + utils::lexical_cast(_meatCont), 1.00f, LeftAlign, BottomAlign);
		Render::EndColor();
		//_meat->DrawRect(FRect(pos.x, pos.x + 64, pos.y + 10, pos.y + 74), FRect(0, 1, 0, 1));
	}
	
};

void MonsterParent::Update(float dt) {
	//Таймер замедления
	if (_slow.y <= 0) {
		_slow = FPoint(0, 0);
	}
	else {
		_slow.y -= dt;
	}
	//Таймер отравления
	if (_decay.y <= 0) {
		_decay = FPoint(0, 0);
	}
	else {
		_decay.y -= dt;
	}
	//Таймер оглушения
	if (_bash.y <= 0) {
		_bash = FPoint(0, 0);
	}
	else {
		_bash.y -= dt;
	}

	_hp -= _decay.x * dt;

	if (_hp <= 0 && !_dying) {
		_dying = true;	
		MM::manager.PlaySample(_dieSound);
	}
	
	PostUpdate(dt);

	if(_dieAnim->IsFinished())
		_dead = true;
	if (_dying && !_dieAnim->IsPlaying()) {
		_dieAnim->setPlayback(true);
		
	}
	float edt = dt;
	edt *= (1 - _slow.x)*(1 - _bash.x);

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
			
			if (_bash != FPoint(0, 0)) {
				_idleAnim->Update(dt);
			}
			else {
				_runAnim->Update(edt*0.4);
			}
			
			
		}
		else {
			_dieAnim->Update(dt);
		}
		
		

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

			
			if (angle > -157.5 && angle <= -112.5) {

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
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);

				}
				
				_lastAngle = -135;
			}

			if (angle > -112.5 && angle <= -67.5) {
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
			if (angle > -67.5 && angle <= -22.5) {
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
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = -45;
			}

			if (angle > -22.5 && angle <= 22.5) {
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
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 0;
			}

			if (angle > 22.5 && angle <= 67.5) {
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
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 45;
			}

			if (angle > 67.5 && angle <= 112.5) {
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
					//_dieAnim->setPlayback(true);

					_runAnim->setLoop(true);
					_runAnim->setPlayback(true);

					
					_idleAnim->setLoop(true);
					_idleAnim->setPlayback(true);
				}
				_lastAngle = 90;
			}

			if (angle > 112.5 && angle <= 157.5) {
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
			if (angle > 157.5 || angle <= -157.5) {
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
				intMap[i][j] = YOU_SHALL_PASS;
				break;
			case SPAWN:
				intMap[i][j] = YOU_SHALL_PASS;
				break;
			case LAIR:
				intMap[i][j] = YOU_PASSED;
				break;
			default:
				intMap[i][j] = YOU_SHALL_NOT_PASS;
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
				//	if (k >= 0 && k < intMap.size()) {
				//		if (l >= 0 && l < intMap[k].size()) {
					if (intMap[k][l] == d - 1) {//если значение их волны меньше на 1
								//и они находятся в окрестности текущей клетки
						if (d > 0 && math::abs(_currentWay[_currentWay.size() - 1].x - k) <= 1 && math::abs(_currentWay[_currentWay.size() - 1].y - l <= 1)) {
							_currentWay.push_back(IPoint(k, l));//добавляем клетку в путь 
							recover = true;
							break;//и прекращаем поиск... 
						}
									
								
							}
							//else if(intMap[k][l]>d && intMap[k][l]<9999) {
							//	intMap[k][l] = 9998;
							//}
							
				//		}
				//	}

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
			if (intMap[i][j]!= YOU_SHALL_NOT_PASS && intMap[i][j]!= YOU_PASSED) {
				intMap[i][j] = YOU_SHALL_PASS;
			}
		}
	}


	//_currentWay.erase(_currentWay.begin());
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
						if (map[k][l] != YOU_PASSED) {
							//и является проходимой и не помеченной волной
							if (map[k][l] == YOU_SHALL_PASS) {
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
	edt *=(1 - _slow.x)*(1 - _bash.x);
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
};

NormalMonster::NormalMonster(MonsterParent::MonsterInfo inf) : MonsterParent() {
	_position =inf._position;
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
};

NormalMonster::~NormalMonster() {
};

void NormalMonster::TakeDamage(TowerType effType, FPoint values, float damage) {
	if (effType == TowerType::SLOW)
		_slow = values;
	if (effType == TowerType::DECAY)
		_decay = values;
	if (effType == TowerType::BASH) {
		values.x *= 100;
		if (math::random(0, 100) <= values.x)
			_bash = FPoint(1, values.y);
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
};

BossMonster::BossMonster(MonsterParent::MonsterInfo inf) : MonsterParent() {
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	_map = inf._map;
	_hp = inf._hp;
	_maxHp = _hp;
	_reduceDamage = inf._reduceDamage;
	_runAnim = inf._runAnim;
	_idleAnim = inf._idleAnim;
	_dieAnim = inf._dieAnim;
	_meat = Core::resourceManager.Get<Render::Texture>("Meat");
	_damage = 5;
	_dieSound = inf._dieSound;
	_meatCont = inf._meat;
};

BossMonster::~BossMonster() {
};

void BossMonster::TakeDamage(TowerType effType, FPoint values, float damage) {
	
	//values.y *= 1 - _reduceDamage;
	
	if (effType == TowerType::SLOW)
		_slow = values;
	if (effType == TowerType::DECAY)
		_decay =FPoint(values.x*4, values.y*4);
	//if (effType == "Bash") {
	//	values.x *= 100;
	//	if (math::random(0, 100) <= values.x)
	//		_bash = FPoint(1, values.y);
	//}

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
};

ImmuneMonster::ImmuneMonster(MonsterParent::MonsterInfo inf) : MonsterParent() {
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
};

ImmuneMonster::~ImmuneMonster() {
};

void ImmuneMonster::TakeDamage(TowerType effType, FPoint values, float damage) {
	
	
	//if (effType == "Slow")
	//	_slow = values;
	if (effType == TowerType::DECAY)
		_decay = values;
	if (effType == TowerType::BASH) {
		values.x *= 100;
		if (math::random(0, 100) <= values.x) {
			_bash = FPoint(1, values.y * 2);
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
};

HealingMonster::HealingMonster(MonsterParent::MonsterInfo inf) : MonsterParent() {
	_position = inf._position;
	_modSpeed = inf._modSpeed;
	_map = inf._map;
	_hp = inf._hp;
	_maxHp = _hp;
	_healPerSecond = inf._healPerSecond;
	_runAnim = inf._runAnim;
	_idleAnim = inf._idleAnim;
	_dieAnim = inf._dieAnim;
	_meat = Core::resourceManager.Get<Render::Texture>("Meat");
	_damage = 1;
	_dieSound = inf._dieSound;
	_meatCont = inf._meat;
};

HealingMonster::~HealingMonster() {
};

void HealingMonster::TakeDamage(TowerType effType, FPoint values, float damage) {
	if (effType == TowerType::SLOW) {
		_slow = FPoint(values.x * 2, values.y*1.5);
		if (_slow.x >= 1) {
			_slow.x = 0.8;
		}
	}
		
	//if (effType == "Decay")
	//	_decay = values;
	if (effType == TowerType::BASH) {
		values.x *= 100;
		if (math::random(0, 100) <= values.x)
			_bash = FPoint(1, values.y);
	}

	if (_hp > 0) {
		_hp -= damage;
		//if (_hp <= 0) {
		//	_dead = true;
		//}
	}
	
	

}

void HealingMonster::PostUpdate(float dt) {
	if (!_dying && _hp < _maxHp) {
		_hp = _hp + _healPerSecond*dt;
		if (_hp > _maxHp)
			_hp = _maxHp;
	}
	

};
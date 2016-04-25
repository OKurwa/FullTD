#include "stdafx.h"
#include <fstream>
#include "FieldMap.h"
#include "FireParent.h"
#include "TowerParent.h"
#include "MonsterParent.h"
#include "TowerPrototypeFactory.h"
#include "TestWidget.h"




TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _curTex(0)
	, _timer(0)
	, _angle(0)
	, _eff(NULL)
	, _scale(0.f)
{
	Init();
}

void TestWidget::Init()
{
	//_fieldMap.LoadFromFile("loadMap.txt");
	_fieldMap.LoadFromXml("NewMap.xml");

	IPoint curCell;
	for (int i = 0; i < _fieldMap.Size().x; i++) {
		for (int j = 0; j < _fieldMap.Size().y; j++) {
			curCell = IPoint(i* _fieldMap.CellSize().x + 32, j* _fieldMap.CellSize().y + 32);
			CellType cell = _fieldMap.PosCellType(curCell);
			if (cell == SPAWN)
				_spawn = IPoint(i, j);
		}
	}
	_texButtons = Core::resourceManager.Get<Render::Texture>("Towers");
	_buildCursor = Core::resourceManager.Get<Render::Texture>("Ghost");
	_towerFactory.Init("Towers.xml");
	//_towerPs.push_back(_towerFactory.createTower(NORMAL));
	//_towerPs.push_back(_towerFactory.createTower(SPLASH));
	//_towerPs.push_back(_towerFactory.createTower(SLOW));
	//_towerPs.push_back(_towerFactory.createTower(DECAY));
	//_towerPs.push_back(_towerFactory.createTower(BASH));
	_enableBuildCursor = false;
	
	_selectedTower = nullptr;
	//_tryMenu = new Menu;
	//_tryMenu->LoadFromXml("Menu.xml");
	_startButton = new Menu;
	_startButton->LoadFromXml("WaveButton.xml");
	IPoint cellPos = _fieldMap.GetSpawnCell();
	IPoint cellSize = _fieldMap.CellSize();
	_startButton->MoveTo(IPoint(cellPos.x*cellSize.x, cellPos.y*cellSize.y));
}

void TestWidget::Draw()
{
	
	IPoint fieldSize = _fieldMap.Size();
	IPoint cellSize = _fieldMap.CellSize();
	

	Render::device.SetTexturing(false);
	_fieldMap.Draw();

	for (unsigned int i = 0; i < _monsters.size();i++) {
		_monsters[i]->Draw();
	}

	for (unsigned int i = 0; i < _monsters.size(); i++) {
		_monsters[i]->DrawMeat();
	}

	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->RangeDraw();
	}
	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->Draw();
	}

	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->UpgradeDraw();
	}
	
	DrawBuildCursor();
	if(World::Instance().State() != WAVE)
		_startButton->Draw();
	
	
}

void TestWidget::Update(float dt)
{

	//_newMenu->Update(dt);
	//anim->Update(dt*0.5);
	if (World::Instance().State() == LOSE || World::Instance().State() == WIN)
		Core::mainScreen.pushLayer("StartLayer");
	//dt *= 0.5;
	_fieldMap.Update(dt);
	for (unsigned int i = 0; i < _monsters.size(); i++) {
		_monsters[i]->Update(dt);
		if (_monsters[i]->Dying()) {
			World::Instance().GoldAdd(_monsters[i]->TakeMonsterMeat());
		}
	}
	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->Update(dt);

	}

	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->TryShoot(_monsters);
	}

	//Задержка спавна монстра
	_spawnTimer += dt;

	if (World::Instance().State() == WAVE) {
		//Cпавн
		MonsterSpawn();
		//Чистка
		MonsterKill();
		//Очистка старой и начало новой
		TryStartNewWave(EndWave());
	}

	//Сброс задержки спавна монстра
	if (_spawnTimer > _spawnTime)
		_spawnTimer = 0;


	World::Instance().Update(dt);
	if (World::Instance().State() == DELAY) {
		_startButton->Buttons()[0][0]->SetCornerText(utils::lexical_cast((int)World::Instance().GetInfo().delayTimer)+" ");
	}
	else {
		_startButton->Buttons()[0][0]->SetCornerText("");
	}
	
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	int a = _startButton->Press(mouse_pos, Button::NO_VALUE);
	if (a == 0 && World::Instance().State() == DELAY) {
		World::Instance().GoldAdd(World::Instance().EarlyStart());
		_startButton->Reset();
	}
	if (a == 0 && World::Instance().State() == START) {
		World::Instance().StartNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[0]);
		_startButton->Reset();
	}
	

	IPoint fieldSize = _fieldMap.Size();
	IPoint cellSize = _fieldMap.CellSize();
	IPoint pos1 = _fieldMap.PosCell(mouse_pos);
	_fieldMap.SelectCell(mouse_pos);
	




	////////////////////////////////////////
	//			 Создание башни           //
	////////////////////////////////////////
	SwitchTowerType(mouse_pos);
	if (World::Instance().State() != WIN && World::Instance().State() != LOSE) {
		if (Core::mainInput.GetMouseLeftButton()) {
			switch (World::Instance().GetTowerType())
			{
			case DESTROY:
				TryDestroyTower(pos1);
				break;
			case EMPTY:
				TryUpgradeTower(pos1, mouse_pos);
				break;
			default:
				TryBuildTower(pos1, cellSize);
				break;
			}
		}
	}
	return false;
}


void TestWidget::MouseMove(const IPoint &mouse_pos)
{
	_startButton->SetLighter(mouse_pos);
	
	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->SetHint(mouse_pos);
	}
	_buildCursorPos = mouse_pos;
}


void TestWidget::MouseUp(const IPoint &mouse_pos)
{
	
}


void TestWidget::AcceptMessage(const Message& message)
{
	//
	// Виджету могут посылаться сообщения с параметрами.
	//
	// Например, при вводе символа с клавиатуры виджетам на активном слое посылается
	// сообщение KeyPress с кодом символа в параметре.
	//
	if (message.getPublisher() == "KeyPress") 
	{
		int code = utils::lexical_cast<int>(message.getData());
		if (code < 0)
		{
			//
			// Отрицательные значения посылаются для виртуальных кодов клавиш,
			// например, -VK_RETURN, -VK_ESCAPE и т.д.
			//
			return;
		}
		
		//
		// Положительные значения посылаются для символов алфавита.
		//
		if (code == 'q')
		{
			Core::appInstance->CloseWindow();
		}
		if (code == 'a')
		{
		}
	}
	else if (message.getPublisher() == "StartLayer") {
		if (message.getIntegerParam() == 1) {
			_fieldMap.Reset();
			_towers.clear();
			_monsters.clear();
			//_monsterAttack.LoadFromFile("loadMob.txt");
			_monsterAttack.LoadFromXml("NewMap.xml");
			_spawnTimer = 0;
			_spawnTime = 0.7;
			_curMonsterAttack = 0;
			_curMonsterCount = 0;
			World::Instance().Init(100, _monsterAttack.GetAttack().size(), _monsterAttack.GetAttack()[0].Count(), _monsterAttack.GetAttack()[0].Count(), 20, _monsterAttack);
		}
		
	} else if(message.getPublisher() == "ChangeGold"){
		if (message.getIntegerParam() == 1) {
			//_tryMenu->SetCurGold(World::Instance().Gold());
			for (unsigned int i = 0; i < _towers.size(); i++) {
				_towers[i]->SetCurGold(World::Instance().Gold());
			}

		}
		
	}
}

//Переключение отображения слотов и курсора
void TestWidget::SwitchTowerType(IPoint mPos){
	if (World::Instance().State() == DELAY || World::Instance().State() == WAVE || World::Instance().State() == START) {
		if (World::Instance().GetTowerType() != EMPTY && World::Instance().GetTowerType() != DESTROY) {
			_enableBuildCursor = true;
		}
		else {
			_enableBuildCursor = false;
		}

		_fieldMap.ShowGhosts(World::Instance().GetTowerType());
	}
};

//Разрушение башни
void TestWidget::TryDestroyTower(IPoint cellPos) {
	if (_fieldMap.DestroyTower(cellPos)) {
		int destrIndex = 0;
		bool found = false;
		for (unsigned int i = 0; i < _towers.size(); i++) {
			if (_towers[i]->Cell() == cellPos) {
				found = true;
				destrIndex = i;
			}
		}
		if (found) {
			World::Instance().GoldAdd(_towers[destrIndex]->Price()*0.75);
			_towers.erase(_towers.begin() + destrIndex);
		}
	};
};

void TestWidget::TryUpgradeTower(IPoint cellPos, IPoint mPos) {
	////////////////////////////////////////
	//				Aпгрейд				  //
	////////////////////////////////////////
	if (_selectedTower) {
		if (Core::mainInput.GetMouseLeftButton() && _selectedTower->UpgradeIRect().Contains(mPos)) {
			if (World::Instance().GoldSpend(_selectedTower->UpgradePrice())) {
				_selectedTower->Upgrade();
			}
			else {
				MM::manager.PlaySample("Denied");
			}
		}
		_selectedTower->SetUpgradeButton(false);
		_selectedTower = nullptr;
	}
	////////////////////////////////////////
	//		Выбор башни для апгрейда	  //
	////////////////////////////////////////
	else if (cellPos != IPoint(-1, -1)) {
		if (!_fieldMap.Cells()[cellPos.x][cellPos.y]->Empty()) {
			for (unsigned int i = 0; i < _towers.size(); i++) {
				if (_towers[i]->Cell() == cellPos && _towers[i]->UpgradePrice()>0) {

					_selectedTower = _towers[i];
					_selectedTower->SetUpgradeButton(true);
				}
				else {
					_towers[i]->SetUpgradeButton(false);
				}
			}
		}
		else {
			for (unsigned int i = 0; i < _towers.size(); i++) {

				_towers[i]->SetUpgradeButton(false);
			}
		}
	}
	else {
		///_selectedTower = nullptr;
		for (unsigned int i = 0; i < _towers.size(); i++) {
			_towers[i]->SetUpgradeButton(false);
		}
	}
};

//Постройка башни
void TestWidget::TryBuildTower(IPoint cellPos, IPoint cSize) {
	if (_fieldMap.AddTower(cellPos)) {
		TowerParent::Ptr t;
		t = _towerFactory.createTower(World::Instance().GetTowerType());

		if (t) {
			if (World::Instance().GoldSpend(t->Price())) {
				t->SetCell(cellPos);
				t->SetPosition(FPoint(cellPos.x*cSize.x + cSize.x / 2, cellPos.y*cSize.y + cSize.y / 2));
				t->SetCurGold(World::Instance().Gold());
				_towers.push_back(t);
			}
			else {
				MM::manager.PlaySample("Denied");
				World::Instance().SetTowerTypeByType(EMPTY);
				//_tryMenu->Reset();
				bool b = _fieldMap.DestroyTower(cellPos);
			}
		}
		else {
			World::Instance().SetTowerTypeByType(EMPTY);
			//_tryMenu->Reset();
			bool b = _fieldMap.DestroyTower(cellPos);
		}
	}
	else if (cellPos != IPoint(-1, -1)) {
		World::Instance().SetTowerTypeByType(EMPTY);
		//_tryMenu->Reset();
		_fieldMap.ShowGhosts(World::Instance().GetTowerType());
	}
};

void TestWidget::DrawBuildCursor() {
	Render::device.SetTexturing(true);
	if (World::Instance().GetTowerType() != EMPTY && World::Instance().GetTowerType() != DESTROY && _enableBuildCursor && _buildCursorPos.x<768) {
		Render::BeginAlphaMul(0.5);
		_buildCursor->Draw(IPoint(_buildCursorPos.x - 32, _buildCursorPos.y - 32));
		Render::EndAlphaMul();
	}
};

void TestWidget::MonsterSpawn() {
	if (_curMonsterAttack < _monsterAttack.GetAttack().size()) {

		if (_monsterAttack.GetAttack()[_curMonsterAttack].Count()>_curMonsterCount && _spawnTimer>_spawnTime) {
			int hp = _monsterAttack.GetAttack()[_curMonsterAttack].MaxHp();
			int spd = _monsterAttack.GetAttack()[_curMonsterAttack].Speed();
			MonsterParent::Ptr m;
			m = _monsterAttack.GetAttackPrototypes()[_curMonsterAttack]->Clone();
			m->SetPosition(FPoint(_spawn.x * _fieldMap.CellSize().x + 32 + math::random(-31, 31), _spawn.y * _fieldMap.CellSize().y + 32 + math::random(-31, 31)), &_fieldMap);

			++_curMonsterCount;
			_monsters.push_back(m);

		}
	}
};

void TestWidget::MonsterKill() {
	for (std::vector<MonsterParent::Ptr>::iterator it = _monsters.begin(); it != _monsters.end();) {
		if ((*it)->Dead() && (*it)->EndDeadAnim()) {
			it = _monsters.erase(it);
		}
		else if ((*it)->Finish()) {
			//World::Instance().GoldAdd(_monsterAttack.GetAttack()[_curMonsterAttack].MGold());


			World::Instance().LoseLife((*it)->Damage());
			it = _monsters.erase(it);
		}
		else {
			it++;
		}

	}
};

bool TestWidget::EndWave() {
	float endWave = true;
	for (unsigned int i = 0; i < _monsters.size(); i++) {
		if (!_monsters[i]->Finish() && !_monsters[i]->Dead())
			endWave = false;
	}
	return endWave;
};

void TestWidget::TryStartNewWave(bool endWave) {
	if (endWave && _curMonsterCount>0 && _monsters.size() == 0) {
		_monsters.clear();
		_curMonsterCount = 0;
		if (_curMonsterAttack + 1<_monsterAttack.GetAttack().size())
			++_curMonsterAttack;
		World::Instance().StartNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[_curMonsterAttack]);
		//World::Instance().SetAttackIndex(_curMonsterAttack);
	}
};
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
	_menuBG = Core::resourceManager.Get<Render::Texture>("MenuBG");


	Menu::MenuInfo info;
	info._rect = IRect(768 +  (1024 - 768 - 128) / 2,44,128,192);
	info._buttonSize = IPoint(64,64);
	info._size = IPoint(2, 3);
	info._buttonsTex = Core::resourceManager.Get<Render::Texture>("Towers");
	
	//TowerParent * t = new TowerParent(FPoint(32, 32), 0, 0, 0, 0, 0, nullptr);
	//_towers.push_back(t);
	
	_towerFactory.Init("NewMap.xml");
	_towerPs.push_back(_towerFactory.createTower(NORMAL));
	_towerPs.push_back(_towerFactory.createTower(SPLASH));
	_towerPs.push_back(_towerFactory.createTower(SLOW));
	_towerPs.push_back(_towerFactory.createTower(DECAY));
	_towerPs.push_back(_towerFactory.createTower(BASH));
	_tryMenu = new Menu(info, _towerPs);
    //anim = Core::resourceManager.Get<Render::Animation>("FireAntAttackAnimation");
	_enableBuildCursor = false;
	_curTowerType = EMPTY;
	_selectedTower = nullptr;
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
		_towers[i]->Draw();
	}

	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->UpgradeDraw();
	}
	
	DrawBuildCursor();
		
	
	//Render::device.PopMatrix();
	_menuBG->Draw(IPoint(768, 0));
	//if (World::Instance().State() != WIN && World::Instance().State() != LOSE && World::Instance().State() != START)
	//	World::Instance().Draw();
	
	_tryMenu->Draw();
	//anim->Draw(IPoint(0, 0));

}

void TestWidget::Update(float dt)
{
	//anim->Update(dt*0.5);
	if (World::Instance().State() == LOSE || World::Instance().State() == WIN)
		dt *= 0;
	//dt *= 0.5;
	_fieldMap.Update(dt);
	for (unsigned int i = 0; i < _monsters.size(); i++) {
		_monsters[i]->Update(dt);
		if (_monsters[i]->Dying()) {
			World::Instance().GoldAdd(_monsters[i]->TakeMonsterMeat());
			Message msg = Message("ChangeGold", 1);
			msg.SetTargetLayer("TestLayer");
			Core::mainScreen.ProcessMessage(msg);
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
	
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{	
	IPoint fieldSize = _fieldMap.Size();
	IPoint cellSize = _fieldMap.CellSize();
	IPoint pos1 = _fieldMap.PosCell(mouse_pos);
	_fieldMap.SelectCell(mouse_pos);
	
	////////////////////////////////////////
	//			 Создание башни           //
	////////////////////////////////////////
	SwitchTowerType(mouse_pos);
	if (World::Instance().State() != WIN && World::Instance().State() != LOSE && World::Instance().State() != START) {
		if (Core::mainInput.GetMouseLeftButton()) {
			switch (_curTowerType)
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
	_tryMenu->SetLighter(mouse_pos);
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
		if (code == '1')
		{
			_curTowerType = NORMAL;
		}
		if (code == '2')
		{
			_curTowerType = SPLASH;
		}
		if (code == '3')
		{
			_curTowerType = SLOW;
		}
		if (code == '4')
		{
			_curTowerType = DECAY;
		}
		if (code == '5')
		{
			_curTowerType = BASH;
		}
		if (code == 's'|| code == 'S') {
			_fieldMap.Reset();
			_towers.clear();
			_monsters.clear();
			//_monsterAttack.LoadFromFile("loadMob.txt");
			_monsterAttack.LoadFromXml("NewMap.xml");
			_spawnTimer = 0;
			_spawnTime = 0.7;
			_curMonsterAttack = 0;
			_curTowerType = EMPTY;
			_curMonsterCount = 0;
			_tryMenu->Reset();
			//for (int i = 0; i < _monsterAttack.GetAttack()[0].Count(); i++) {
			//	_monsters[i]->FindAWay();
			//}

			//_fieldMap.TryInit();
			//_fieldMap.SaveToFile("save.txt");

			World::Instance().Init(100, _monsterAttack.GetAttack().size(), _monsterAttack.GetAttack()[0].Count(), _monsterAttack.GetAttack()[0].Count(), 20, _monsterAttack);
			World::Instance().StartNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[0]);
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
			_curTowerType = EMPTY;
			_curMonsterCount = 0;
			_tryMenu->Reset();
			

			World::Instance().Init(100, _monsterAttack.GetAttack().size(), _monsterAttack.GetAttack()[0].Count(), _monsterAttack.GetAttack()[0].Count(), 20, _monsterAttack);
			World::Instance().StartNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[0]);
			_tryMenu->SetCurGold(World::Instance().Gold());
			for (unsigned int i = 0; i < _towers.size(); i++) {
				_towers[i]->SetCurGold(World::Instance().Gold());
			}
		}
		
	} else if(message.getPublisher() == "ChangeGold"){
		if (message.getIntegerParam() == 1) {
			_tryMenu->SetCurGold(World::Instance().Gold());
			for (unsigned int i = 0; i < _towers.size(); i++) {
				_towers[i]->SetCurGold(World::Instance().Gold());
			}

		}
		
	}
}


void TestWidget::SwitchTowerType(IPoint mPos){
	if (World::Instance().State() == DELAY || World::Instance().State() == WAVE) {
		_curTowerType = _tryMenu->Press(mPos, _curTowerType);
		if (_curTowerType != EMPTY && _curTowerType != DESTROY) {
			_enableBuildCursor = true;
		}
		else {
			_enableBuildCursor = false;
		}

		_fieldMap.ShowGhosts(_curTowerType);
	}
};

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
			Message msg = Message("ChangeGold", 1);
			msg.SetTargetLayer("TestLayer");
			Core::mainScreen.ProcessMessage(msg);
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
				Message msg = Message("ChangeGold", 1);
				msg.SetTargetLayer("TestLayer");
				Core::mainScreen.ProcessMessage(msg);
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

void TestWidget::TryBuildTower(IPoint cellPos, IPoint cSize) {
	if (_fieldMap.AddTower(cellPos)) {
		TowerParent::Ptr t;
		t = _towerFactory.createTower(_curTowerType);

		if (t) {
			if (World::Instance().GoldSpend(t->Price())) {
				t->SetCell(cellPos);
				t->SetPosition(FPoint(cellPos.x*cSize.x + cSize.x / 2, cellPos.y*cSize.y + cSize.y / 2));
				_towers.push_back(t);
				Message msg = Message("ChangeGold", 1);
				msg.SetTargetLayer("TestLayer");
				Core::mainScreen.ProcessMessage(msg);
			}
			else {
				MM::manager.PlaySample("Denied");
				_curTowerType = EMPTY;
				_tryMenu->Reset();
				bool b = _fieldMap.DestroyTower(cellPos);
			}
		}
		else {
			_curTowerType = EMPTY;
			_tryMenu->Reset();
			bool b = _fieldMap.DestroyTower(cellPos);
		}
	}
	else if (cellPos != IPoint(-1, -1)) {
		_curTowerType = EMPTY;
		_tryMenu->Reset();
		_fieldMap.ShowGhosts(_curTowerType);
	}
};

void TestWidget::DrawBuildCursor() {
	Render::device.SetTexturing(true);
	if (_curTowerType != EMPTY && _curTowerType != DESTROY && _enableBuildCursor && _buildCursorPos.x<768) {
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
			m = _monsterAttack.GetAttackPrototypes()[_curMonsterAttack]->clone();
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
		Message msg = Message("ChangeGold", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		//World::Instance().SetAttackIndex(_curMonsterAttack);
	}
};
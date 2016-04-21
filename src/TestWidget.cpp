#include "stdafx.h"
#include <fstream>
#include "FieldMap.h"
#include "FireParent.h"
#include "TowerParent.h"
#include "MonsterParent.h"
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
	_towerPs.push_back(_towerFactory.createNormal());
	_towerPs.push_back(_towerFactory.createSplash());
	_towerPs.push_back(_towerFactory.createSlow());
	_towerPs.push_back(_towerFactory.createDecay());
	_towerPs.push_back(_towerFactory.createBash());
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
	Render::device.SetTexturing(true);
	if (_curTowerType != EMPTY && _curTowerType != DESTROY && _enableBuildCursor && _buildCursorPos.x<768) {
		Render::BeginAlphaMul(0.5);
		_buildCursor->Draw(IPoint(_buildCursorPos.x - 32, _buildCursorPos.y - 32));
		Render::EndAlphaMul();
	}

		
	
	//Render::device.PopMatrix();
	_menuBG->Draw(IPoint(768, 0));
	World::Instance().Draw();
	
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
	}
	for (unsigned int i = 0; i < _towers.size(); i++) {
		_towers[i]->Update(dt);
		_towers[i]->SetCurGold(World::Instance().Gold());
	}

	for (unsigned int i = 0; i < _towers.size(); i++) {
		//if (_towers[i]->TakeAim(_monsters)) {
		//	_towers[i]->Shoot();
		//}
		_towers[i]->TryShoot(_monsters);
	}

	//Задержка спавна монстра
	_spawnTimer += dt;

	if (World::Instance().State() == WAVE) {
		//Cпавн
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

		for (std::vector<MonsterParent::Ptr>::iterator it = _monsters.begin(); it != _monsters.end();) {
			if ((*it)->Dead() && (*it)->EndDeadAnim()) {
				World::Instance().GoldAdd(_monsterAttack.GetAttack()[_curMonsterAttack].MGold());
				it = _monsters.erase(it);
			}else if((*it)->Finish()) {
				//World::Instance().GoldAdd(_monsterAttack.GetAttack()[_curMonsterAttack].MGold());
				
				
				World::Instance().LoseLife((*it)->Damage());
				it = _monsters.erase(it);
			}
			else {
				it++;
			}

		}
		//Проверка окончания волны
		float endWave = true;
		for (unsigned int i = 0; i < _monsters.size(); i++) {
			if (!_monsters[i]->Finish() && !_monsters[i]->Dead())
				endWave = false;
		}
		//Очистка старой и начало новой
		if (endWave && _curMonsterCount>0 && _monsters.size() == 0) {
			_monsters.clear();
			_curMonsterCount = 0;
			if(_curMonsterAttack+1<_monsterAttack.GetAttack().size())
 				++_curMonsterAttack;
			World::Instance().SetNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[_curMonsterAttack]);
			//World::Instance().SetAttackIndex(_curMonsterAttack);
		}
		
	}

	//Сброс задержки спавна монстра
	if (_spawnTimer > _spawnTime)
		_spawnTimer = 0;
	
	
	World::Instance().Update(dt);
	_tryMenu->SetCurGold(World::Instance().Gold());
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
	if (World::Instance().State() == DELAY || World::Instance().State() == WAVE) {
		_curTowerType = _tryMenu->Press(mouse_pos, _curTowerType);
		if (_curTowerType != EMPTY && _curTowerType != DESTROY) {
			_enableBuildCursor = true;
		}
		else {
			_enableBuildCursor = false;
		}

		_fieldMap.ShowGhosts(_curTowerType);
	}
	

	if (World::Instance().State() != WIN && World::Instance().State() != LOSE && World::Instance().State() != START) {
		
		

		if (Core::mainInput.GetMouseLeftButton()) {


			switch (_curTowerType)
			{
			case DESTROY:
				if (_fieldMap.DestroyTower(pos1)) {
					int destrIndex = 0;
					bool found = false;
					for (unsigned int i = 0; i < _towers.size(); i++) {
						if (_towers[i]->Cell() == pos1) {
							found = true;
							destrIndex = i;
						}
					}
					if (found) {
						World::Instance().GoldAdd(_towers[destrIndex]->Price()*0.75);
						_towers.erase(_towers.begin() + destrIndex);
					}
				};
				break;



			case EMPTY:
				////////////////////////////////////////
				//				Aпгрейд				  //
				////////////////////////////////////////
				if (_selectedTower) {
					if (Core::mainInput.GetMouseLeftButton() && _selectedTower->UpgradeIRect().Contains(mouse_pos)) {
						if (World::Instance().GoldSpend(_selectedTower->UpgradePrice())) {
							_selectedTower->Upgrade();
						}
						else {
							MM::manager.PlaySample("Denied");
						}
					}
					_selectedTower->SetUpgradeButton(false);
					_selectedTower = nullptr;
				}else

				////////////////////////////////////////
				//		Выбор башни для апгрейда	  //
				////////////////////////////////////////
				if (pos1 != IPoint(-1, -1)) {


					if (!_fieldMap.Cells()[pos1.x][pos1.y]->Empty()) {
						for (unsigned int i = 0; i < _towers.size(); i++) {
							if (_towers[i]->Cell() == pos1 && _towers[i]->UpgradePrice()>0) {

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




				


				

				break;
			
			
			
			default:
				if (_fieldMap.AddTower(pos1)) {

					TowerParent::Ptr t;
					switch (_curTowerType)
					{
					case NORMAL:
						t = _towerFactory.createNormal();
						break;
					case SPLASH:
						t = _towerFactory.createSplash();
						break;
					case SLOW:
						t = _towerFactory.createSlow();
						break;
					case DECAY:
						t = _towerFactory.createDecay();
						break;
					case BASH:
						t = _towerFactory.createBash();
						break;
					}
					if (t) {
						if (World::Instance().GoldSpend(t->Price())) {
							t->SetCell(pos1);
							t->SetPosition(FPoint(pos1.x*cellSize.x + cellSize.x / 2, pos1.y*cellSize.y + cellSize.y / 2));
							_towers.push_back(t);

						}
						else {
							MM::manager.PlaySample("Denied");
							_curTowerType = EMPTY;
							_tryMenu->Reset();
							bool b = _fieldMap.DestroyTower(pos1);
							
						}
					}
					else {
						_curTowerType = EMPTY;
						_tryMenu->Reset();
						bool b = _fieldMap.DestroyTower(pos1);
						
					}


				}
				else if (pos1 != IPoint(-1, -1)) {
					_curTowerType = EMPTY;
					_tryMenu->Reset();
					_fieldMap.ShowGhosts(_curTowerType);
				}

					
				

				break;
			}
			
			

		}
	}






	////////////////////////////////////////
	//				Aпгрейд				  //
	////////////////////////////////////////
/*
	if (_selectedTower) {
		if (Core::mainInput.GetMouseLeftButton() && _selectedTower->UpgradeIRect().Contains(mouse_pos)) {
			if (World::Instance().GoldSpend(_selectedTower->UpgradePrice())) {
				_selectedTower->Upgrade();
				_selectedTower->SetUpgradeButton(false);
				_selectedTower = nullptr;
			}
			else {
				_selectedTower->SetUpgradeButton(false);
				_selectedTower = nullptr;
			}
		}
	}
	*/

	





	////////////////////////////////////////
	//		Выбор башни для апгрейда	  //
	////////////////////////////////////////
	/*
	if (Core::mainInput.GetMouseLeftButton()) {


		if (pos1 != IPoint(-1, -1) && _curTowerType == EMPTY) {


			if (!_fieldMap.Cells()[pos1.x][pos1.y]->Empty()) {
				for (int i = 0; i < _towers.size(); i++) {
					if (_towers[i]->Cell() == pos1 && _towers[i]->UpgradePrice()>0) {
						
						_selectedTower = _towers[i];

					}
					else {
						for (int i = 0; i < _towers.size(); i++) {
							_towers[i]->SetUpgradeButton(false);
						}
					}
				}
			}






		}
		else {
			_selectedTower = nullptr;
			for (int i = 0; i < _towers.size(); i++) {
				_towers[i]->SetUpgradeButton(false);
			}
		}
		if (_selectedTower) {
			_selectedTower->SetUpgradeButton(true);
		}
		
		//else if (_curTowerType != EMPTY) {
		//	_curTowerType = EMPTY;
		//	_fieldMap.ShowGhosts(_curTowerType);
		//	_tryMenu->Reset();
		//}

	}

	*/





	
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





			World::Instance().SetNewAttack(_monsterAttack.Delay(), _monsterAttack.GetAttack()[0]);
		}
		
	}
}

bool TestWidget::LoadMapFromXml(std::string filename) {

	




	try {
		rapidxml::file<> file(filename.c_str());
		// Может бросить исключение, если нет файла.

		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());
		// Может бросить исключение, если xml испорчен.

		rapidxml::xml_node<>* root = doc.first_node();
		if (!root) { Assert(false); throw std::runtime_error("No root node"); }

		rapidxml::xml_node<>* game = root->first_node("Game");
		for (; game != NULL; game = game->next_sibling("Game")) {
			rapidxml::xml_node<>* r = game->first_node();
			for (; r != NULL; r = r->next_sibling()) {
				if (utils::equals(r->name(), "Map")) {
					
				}
			}
		}
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
	return false;
};
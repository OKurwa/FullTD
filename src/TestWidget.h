#pragma once
//#include "Firework.h"
#include "FieldMap.h"
#include "MonsterAttack.h"
#include "FireParent.h"
#include "TowerParent.h"
#include "MonsterParent.h"
#include "World.h"
#include "Menu.h"
#include "TowerPrototypeFactory.h"
///
/// ������ - �������� ���������� ������� �� ������.
/// �� ������������ ����, � ����� ����� ��������� ������ �������.
///

class TestWidget : public GUI::Widget
{
public:
	TestWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw();
	void Update(float dt);
	
	void AcceptMessage(const Message& message);
	
	bool MouseDown(const IPoint& mouse_pos);
	void MouseMove(const IPoint& mouse_pos);
	void MouseUp(const IPoint& mouse_pos);
	
	//�������� �� �����
	void SwitchTowerType(IPoint);
	void TryDestroyTower(IPoint);
	void TryUpgradeTower(IPoint,IPoint);
	void TryBuildTower(IPoint, IPoint);

	//�������� � �������
	void MonsterSpawn();
	void MonsterKill();
	bool EndWave();
	void TryStartNewWave(bool);

	//�������� � Draw
	void DrawBuildCursor();

private:
	void Init();

private:
	float _timer;
	
	float _scale;
	float _angle;
	
	Render::Texture* _tex1;
	Render::Texture* _tex2;
	Render::TexturePtr _menuBG;
	Render::TexturePtr _texButtons;
	Render::TexturePtr _destroyCursor;
	bool _enableBuildCursor;
	Render::TexturePtr _buildCursor;

	IPoint _buildCursorPos;
	int _curTex;

	EffectsContainer _effCont;
	ParticleEffectPtr _eff;
	Render::Animation* anim; 
	
	


	IPoint _spawn;
	float _spawnTimer;
	float _spawnTime;
	int   _curMonsterAttack;
	int   _curMonsterCount;
	TowerPrototypeFactory _towerFactory;
	TowerParent::Ptr _selectedTower;
	std::vector<TowerParent::Ptr> _towerPs;
	std::vector<TowerParent::Ptr> _towers;     // ���� ����� �� ����
	std::vector<MonsterParent::Ptr> _monsters;    // ������� ������� �� ����
	FieldMap _fieldMap;
	MonsterAttack _monsterAttack;
	TowerType _curTowerType;
	


	
};

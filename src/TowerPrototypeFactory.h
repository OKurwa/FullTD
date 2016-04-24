#pragma once


// Фабрика для создания башен
class TowerPrototypeFactory
{
public:
	TowerPrototypeFactory();
	void Init(std::string filename);
	TowerParent::Ptr createTower(TowerType tType);

private:
	NormalTower _nPrototype;
	SlowTower _slPrototype;
	SplashTower _spPrototype;
	BashTower _bPrototype;
	DecayTower _dPrototype;
};
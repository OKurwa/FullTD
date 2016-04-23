#pragma once


// Фабрика для создания башен
class TowerPrototypeFactory
{
public:
	TowerPrototypeFactory();
	void Init(std::string filename);
	TowerParent::Ptr createTower(TowerType tType);

private:
	bool _Nloaded;
	bool _Slloaded;
	bool _Sploaded;
	bool _Bloaded;
	bool _Dloaded;
	NormalTower _nPrototype;
	SlowTower _slPrototype;
	SplashTower _spPrototype;
	BashTower _bPrototype;
	DecayTower _dPrototype;
};
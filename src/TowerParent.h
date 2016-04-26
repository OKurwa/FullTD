#pragma once
#include "MonsterParent.h"
#include "FireParent.h"
//////////////////////////////
//--------------------------//
//----------Пушка-----------//
//--------------------------//
//////////////////////////////
//----------------------------------------------//
//----------------------------------------------//
//			Базовый класс башни 				//
//----------------------------------------------//
//----------------------------------------------//
class TowerParent : public RefCounter
{
public:
	typedef boost::intrusive_ptr<TowerParent> Ptr;
	TowerParent();
	
	~TowerParent();
	void RangeDraw();
	void Draw();
	void UpgradeDraw();
	void Update(float dt);
	void TowerParent::UpdateAnimAngle(MonsterParent::Ptr _target);
	virtual Ptr Clone() = 0;
	IPoint Cell();
	virtual void SetPosition(FPoint);
	void TowerParent::SetUButtonPosition();
	void SetCell(IPoint);
	void SetUpgradeButton(bool);
	IRect UpgradeIRect();
	void Upgrade();
	int UpgradePrice();
	virtual void TryShoot(std::vector<MonsterParent::Ptr> & monsters) = 0;
	void TakeTarget(std::vector<MonsterParent::Ptr> & monsters, FireParent::Ptr mis);
	int Price();
	void SetCurGold(int);
	void SetHint(IPoint);
	virtual void DrawHintText(IRect) {};
	void LoadTowerFormXML(rapidxml::xml_node<>* towerNode);
	
protected:

	

	
	TowerType _towerType;
	IPoint _damage;
	FPoint _position;
	IPoint _cell;
	IPoint _cellSize;
	MonsterParent::Ptr _target;
	float  _reloadTime;
	float  _reloadTimer;
	int	   _range;
	int    _missileSpeed;
	int    _price;
	int    _curGold;
	int	   _lvl;
	int    _lvlCount;
	bool   _showUpgradeButton;
	IRect  _upgradeButtonRect;
	Render::TexturePtr _upTex;
	std::vector<FireParent::Ptr> _missiles;
	std::vector<FireParent::MissInfo> _missilesPrototypes;
	
	//Визуальная
	Render::TexturePtr _texHint;
	static const IRect _hintTexRect;
	Render::AnimationPtr _idleAnim;
	Render::AnimationPtr _atkAnim;
	AnimAngles _attackAnimAngles;
	AnimAngles _idleAnimAngles;

	ParticleEffectPtr _upEff;
	EffectsContainer _upCont;

	Render::TexturePtr _texCircle;
	bool _hint;
};


//----------------------------------------------//
//----------------------------------------------//
//				Обычная башня	 				//
//----------------------------------------------//
//----------------------------------------------//
class NormalTower: public TowerParent
{
	friend class TowerPrototypeFactory;
public:
	
	NormalTower();
	NormalTower(NormalTower&);
	
	~NormalTower();
	TowerParent::Ptr Clone() {

		return new NormalTower(*this);
	}

	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	
	void DrawHintText(IRect);
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Замедляющая башня	 			//
//----------------------------------------------//
//----------------------------------------------//
class SlowTower : public TowerParent
{
	friend class TowerPrototypeFactory;
public:
	SlowTower();
	SlowTower(SlowTower& proto);
	
	~SlowTower();

	TowerParent::Ptr Clone() {
		return new SlowTower(*this);
	}
		
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	
	void DrawHintText(IRect);
	
private:
	int    _splashRange;
	FPoint _slow;
	std::vector<MonsterParent::Ptr> _targets;
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Отравляющая башня	 			//
//----------------------------------------------//
//----------------------------------------------//
class DecayTower : public TowerParent
{
	friend class TowerPrototypeFactory;
public:
	DecayTower();
	DecayTower(DecayTower& proto);
	
	~DecayTower();
	TowerParent::Ptr Clone() {
		return new DecayTower(*this);
	}

	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	
	void DrawHintText(IRect);
	
private:
	FPoint _decay;
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Оглушающая башня	 			//
//----------------------------------------------//
//----------------------------------------------//
class BashTower : public TowerParent
{
	friend class TowerPrototypeFactory;
public:
	BashTower();
	BashTower(BashTower& proto);
	
	~BashTower();
	TowerParent::Ptr Clone() {

		return new BashTower(*this);
	}
		
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	
	void DrawHintText(IRect);
private:
	FPoint _bash;
	
};


//----------------------------------------------//
//----------------------------------------------//
//				Разрывная башня	 				//
//----------------------------------------------//
//----------------------------------------------//
class SplashTower : public TowerParent
{
	friend class TowerPrototypeFactory;
public:
	SplashTower();
	SplashTower(SplashTower& proto);
	
	~SplashTower();
	TowerParent::Ptr Clone() {
		return new SplashTower(*this);
	}

	void TryShoot(std::vector<MonsterParent::Ptr> & monsters) ;
	
	void DrawHintText(IRect);
private:
	int    _splashRange;
	std::vector<MonsterParent::Ptr> _targets;
	
};







const AnimAngles ATK_ANGLES = {
	IPoint(12, 15),
	IPoint(44, 47),
	IPoint(76, 79),
	IPoint(108, 111),
	IPoint(140, 143),
	IPoint(172, 175),
	IPoint(204, 207),
	IPoint(236, 239) };
const AnimAngles IDL_ANGLES = {
	IPoint(0,3),
	IPoint(32,35),
	IPoint(64,67),
	IPoint(96,99),
	IPoint(128,131),
	IPoint(160,163),
	IPoint(192,195),
	IPoint(224,227) };


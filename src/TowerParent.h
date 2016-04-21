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
class TowerParent 
{
public:
	typedef boost::intrusive_ptr<TowerParent> Ptr;
	TowerParent();
	TowerParent(FPoint position, IPoint cell, float rTime, float rTimer, int range, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~TowerParent();
	void Draw();
	void UpgradeDraw();
	void Update(float dt);
	void TowerParent::UpdateAnimAngle(MonsterParent * _target);
	virtual Ptr clone() = 0;
	std::vector<FireParent::Ptr> & GetMissiles();
	FPoint Position();
	IPoint Cell();
	virtual void SetPosition(FPoint);
	void TowerParent::SetUButtonPosition();
	void SetCell(IPoint);
	void SetUpgradeButton(bool);
	bool UpgradeButtonActive();
	IRect UpgradeIRect();
	void Upgrade();
	virtual int UpgradePrice() = 0 ;
	virtual void TryShoot(std::vector<MonsterParent::Ptr> & monsters) = 0;
	friend void intrusive_ptr_add_ref(TowerParent*);
	friend void intrusive_ptr_release(TowerParent*);
	void AddRef() {
		++ref_cnt_;
	}
	void Release() {
		if (--ref_cnt_ == 0) {
			delete this;

		}
	}
	void SetPrice(int);
	int Price();
	void SetCurGold(int);
	void SetHint(IPoint);
	virtual void DrawHintText(IRect) {};
protected:

	int ref_cnt_;

	
	TowerType _towerType;
	IPoint _damage;
	FPoint _position;
	IPoint _cell;
	MonsterParent * _target;
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
	
	
	//Визуальная
	Render::TexturePtr _texHint;
	Render::AnimationPtr _idleAnim;
	Render::AnimationPtr _atkAnim;
	AnimAngles _attackAnimAngles;
	AnimAngles _idleAnimAngles;

	ParticleEffectPtr _upEff;
	EffectsContainer _upCont;


	bool _hint;
};
inline void intrusive_ptr_add_ref(TowerParent* e) { e->AddRef(); }
inline void intrusive_ptr_release(TowerParent* e) { e->Release(); }

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
	NormalTower(FPoint position, IPoint cell, float rTime, float rTimer, int range, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~NormalTower();
	TowerParent::Ptr clone() {

		return new NormalTower(*this);
	}

	//void Draw();
	//void Update(float dt);
	//bool Shoot();
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	void LoadFromXml(std::string filename);
	void SetPosition(FPoint);
	int  UpgradePrice();
	void DrawHintText(IRect);
private:
	std::vector<NormalMissile::NMissInfo> _missilesPrototypes;
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
	SlowTower(FPoint position, IPoint cell, std::vector<MonsterParent::Ptr> & targets, float rTime, float rTimer, int range, int sRange, FPoint sFactor, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~SlowTower();

	TowerParent::Ptr clone() {
		return new SlowTower(*this);
	}
	//void Draw();
	//void Update(float dt);
	
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	void LoadFromXml(std::string filename);
	
	void SetPosition(FPoint);
	int  UpgradePrice();
	void DrawHintText(IRect);
private:
	int    _splashRange;
	FPoint _slow;
	std::vector<MonsterParent::Ptr> _targets;
	std::vector<SlowMissile::SlMissInfo> _missilesPrototypes;
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
	DecayTower(FPoint position, IPoint cell, float rTime, float rTimer, int range, FPoint decay, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~DecayTower();
	TowerParent::Ptr clone() {
		return new DecayTower(*this);
	}

	//void Draw();
	//void Update(float dt);

	
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	void LoadFromXml(std::string filename);
	void SetPosition(FPoint);
	int  UpgradePrice();
	void DrawHintText(IRect);
private:
	FPoint _decay;
	std::vector<DecayMissile::DMissInfo> _missilesPrototypes;
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
	BashTower(FPoint position, IPoint cell, float rTime, float rTimer, int range, FPoint bash, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~BashTower();
	TowerParent::Ptr clone() {

		return new BashTower(*this);
	}

	//void Draw();
	//void Update(float dt);

	
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters);
	void LoadFromXml(std::string filename);
	void SetPosition(FPoint);
	int  UpgradePrice();
	void DrawHintText(IRect);
private:
	FPoint _bash;
	std::vector<BashMissile::BMissInfo> _missilesPrototypes;
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
	SplashTower(FPoint position, IPoint cell, std::vector<MonsterParent::Ptr> & targets, float rTime, float rTimer, int range, int sRange, int mSpeed, IPoint dmg, Render::TexturePtr tex);
	~SplashTower();
	TowerParent::Ptr clone() {
		return new SplashTower(*this);
	}

	//void Draw();
	//void Update(float dt);

	
	void TryShoot(std::vector<MonsterParent::Ptr> & monsters) ;
	void LoadFromXml(std::string filename);
	
	void SetPosition(FPoint);
	int  UpgradePrice();
	void DrawHintText(IRect);
private:
	int    _splashRange;
	std::vector<MonsterParent::Ptr> _targets;
	std::vector<SplashMissile::SpMissInfo> _missilesPrototypes;
};




// Фабрика для создания башен
class TowerPrototypeFactory
{
public:
	TowerPrototypeFactory() {
		_Nloaded = false;
		_Slloaded = false;
		_Sploaded = false;
		_Bloaded = false;
		_Dloaded = false;
	}
	TowerParent::Ptr createNormal() {
		static NormalTower prototype;
		if (!_Nloaded) {
			prototype.LoadFromXml("NewMap.xml");
			_Nloaded = true;
		}
		
		return prototype.clone();
	}
	TowerParent::Ptr createSlow() {
		static SlowTower prototype;
		if (!_Slloaded) {
			prototype.LoadFromXml("NewMap.xml");
			_Slloaded = true;
		}
		return prototype.clone();
	}
	TowerParent::Ptr createSplash() {
		static SplashTower prototype;
		if (!_Sploaded) {
			prototype.LoadFromXml("NewMap.xml");
			_Sploaded = true;
		}
		return prototype.clone();
	}
	TowerParent::Ptr createBash() {
		static BashTower prototype;
		if (!_Bloaded) {
			prototype.LoadFromXml("NewMap.xml");
			_Bloaded = true;
		}
		return prototype.clone();
	}
	TowerParent::Ptr createDecay() {
		static DecayTower prototype;
		if (!_Dloaded) {
			prototype.LoadFromXml("NewMap.xml");
			_Dloaded = true;
		}
		return prototype.clone();
	}

private:
	bool _Nloaded;
	bool _Slloaded;
	bool _Sploaded;
	bool _Bloaded;
	bool _Dloaded;
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

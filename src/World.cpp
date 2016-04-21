#include "stdafx.h"
#include "World.h"


World::World() {
	_start_w = Core::resourceManager.Get<Render::Texture>("StartWindow");
	_lose_w = Core::resourceManager.Get<Render::Texture>("LoseWindow");
	_win_w = Core::resourceManager.Get<Render::Texture>("WinWindow");
};
World::~World() {};


void World::Init(	int gold,
			int attacksRemaining,
			int nextAttackMonstersCount,
			int monstersRemaining,
			int lives,
			MonsterAttack & attacks) {
	_gold = gold;
	_attacksRemaining = attacksRemaining;
	_nextAttackMonstersCount = nextAttackMonstersCount;
	_monstersRemaining = monstersRemaining;
	_lives = lives;
	_curAttackIndex = 0;
	//_attacks = attacks;
	if (attacks.GetAttack().size() >= 2) {
		_curAttackName = attacks.GetAttack()[_curAttackIndex].Name();
		_nextAttackName =attacks.GetAttack()[_curAttackIndex + 1].Name();
		_curAttackType = attacks.GetAttack()[_curAttackIndex].Type();
		_nextAttackType = attacks.GetAttack()[_curAttackIndex + 1].Type();
	}
	else if (attacks.GetAttack().size() == 1) {
		_curAttackName = attacks.GetAttack()[_curAttackIndex].Name();
		_nextAttackName = "None";
		_curAttackType = attacks.GetAttack()[_curAttackIndex].Type();
		_nextAttackType = "None";
	}
	else {
		_curAttackName = "None";
		_nextAttackName = "None";
		_curAttackType = "None";
		_nextAttackType = "None";
	}
	
	_state = START;
	
};

std::string World::CurAttackName() {
	return _curAttackName;
};

std::string World::NextAttackName() {
	return _nextAttackName;
};

std::string World::CurAttackType() {
	return _curAttackType;
};

std::string World::NextAttackType() {
	return _nextAttackType;
};

int World::Gold() {
	return _gold;
};

WorldState World::State() {
	return _state;
};

void World::SetState(WorldState state) {
	_state = state;
};

void World::SetAttackIndex(int index) {
	_curAttackIndex = index;
};

void World::SetNewAttack(float delay, Attack & atk) {
	
	if(_attacksRemaining>0){
		_state = DELAY;
		_delayTimer = delay;
		GoldAdd(atk.WGold());
		++_curAttackIndex;
		--_attacksRemaining;
	}
	else{
		if(_lives > 0)
			_state = WIN;
	}
	
	_curAttackName = atk.Name();
	_curAttackType = atk.Type();
	_maxHP = atk.MaxHp();
	_speed = atk.Speed();
	_gpm = atk.MGold();
		
};

void World::Draw() {
	switch (_state)
	{
	case START:
		Render::BindFont("arial");
		Render::device.SetTexturing(true);

		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(838, 685), "HUNGRY ANTS TD", 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(896, 335), "MEAT", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		Render::device.SetTexturing(false);
		Render::BeginAlphaMul(0.5);
		Render::BeginColor(Color(0, 0, 0, 255));
		Render::DrawRect(0, 0, 1024, 768);
		Render::EndColor();
		Render::EndAlphaMul();
		Render::device.SetTexturing(true);
		_start_w->Draw(FPoint(110, 180));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(_state), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(384,530), "HUNGRY ANTS TD", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384,440), "Help big hungry ants to get some meat.", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 390), "Press 's' to start the hunt!", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		
		break;
	case WIN:
		Render::BindFont("arial");
		Render::device.SetTexturing(true);

		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(838, 685), "HUNGRY ANTS TD", 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(896, 335), "MEAT", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		Render::device.SetTexturing(false);
		Render::BeginAlphaMul(0.5);
		Render::BeginColor(Color(0, 0, 0, 255));
		Render::DrawRect(0, 0, 1024, 768);
		Render::EndColor();
		Render::EndAlphaMul();
		Render::device.SetTexturing(true);
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(_state), 1.00f, LeftAlign, BottomAlign);
		Render::EndColor();
		_win_w->Draw(FPoint(110, 180));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(_state), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(384, 530), "VICTORY!", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 440), "Now your ants became bigger and hungrier! ", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 420), "Do you want to feed them more?", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 390), "Press 's' to start new hunt!", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		break;
	case LOSE:
		Render::BindFont("arial");
		Render::device.SetTexturing(true);

		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(838, 685), "HUNGRY ANTS TD", 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(896, 335), "MEAT", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		Render::device.SetTexturing(false);
		Render::BeginAlphaMul(0.5);
		Render::BeginColor(Color(0, 0, 0, 255));
		Render::DrawRect(0, 0, 1024, 768);
		Render::EndColor();
		Render::EndAlphaMul();

		Render::device.SetTexturing(true);
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(_state), 1.00f, LeftAlign, BottomAlign);
		Render::EndColor();
		_lose_w->Draw(FPoint(110, 180));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(_state), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(384, 530), "DEFEAT!", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 440), "Now your ants became small and vegan!", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 390), "Press 's' to make them hunt again!", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		break;
	default:
		Render::BindFont("arial");
		Render::device.SetTexturing(true);

		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(838, 685), "HUNGRY ANTS TD", 1.00f, LeftAlign, BottomAlign);

		Render::PrintString(FPoint(828, 600), "HP : " + utils::lexical_cast(_lives), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(828, 580), "WAVE BEGINS IN : " + utils::lexical_cast(math::round(_delayTimer)) + "sec", 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(828, 560), "WAVES REMAINING : " + utils::lexical_cast(_attacksRemaining), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(896, 520), "CURRENT WAVE : " + utils::lexical_cast(_curAttackIndex), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 505), "Name : " + _curAttackName, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 490), "Type: " + _curAttackType, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 475), "Monster HP : " + utils::lexical_cast(_maxHP), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 460), "Monster speed : " + utils::lexical_cast(_speed), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 445), "Meat per monster : " + utils::lexical_cast(_gpm), 1.00f, CenterAlign, BottomAlign);

		Render::PrintString(FPoint(896, 335), "MEAT", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 300), utils::lexical_cast(_gold), 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		Render::device.SetTexturing(false);
		break;
	}

	
		
	
	
};

void World::Update(float dt) {
	
	if (_lives <= 0)
		_state = LOSE;
	if (_lives == 0 && _attacksRemaining == 0 && _nextAttackMonstersCount == 0 && _monstersRemaining == 0)
		_state = START;
	
	if (_state == DELAY) {
		if (_delayTimer > 0) {
			_delayTimer -= dt;
		}
		else {
			_delayTimer = 0;
			_state = WAVE;
		}
	}

};
void World::GoldAdd(int gold) {
	_gold += gold;
};

bool World::GoldSpend(int gold) {
	if (gold > _gold) {
		return false;
	}
	else{
		_gold -= gold;
		return true;
	}
};

void World::LoseLife(int i) {
	_lives-=i;
};

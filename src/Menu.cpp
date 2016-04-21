#include "stdafx.h"
#include "Menu.h"
#include "TowerParent.h"
Button::Button() {
	_tex = nullptr;
	_rect = IRect(0,0,0,0);
	_buttonUVRect  = FRect(0,0,0,0);
	_pos = FPoint(0, 0);
	_menuPos = IPoint(0,0);
	_size = IPoint(0, 0);
	_type = EMPTY;
	_empty = true;
	_light = false;
	_pressed = false;

};
Button::Button(ButtonInfo info) {
	_tex = info._tex;
	_texHint = Core::resourceManager.Get<Render::Texture>("Hint");
	_buttonUVRect = info._buttonUVRect;
	_pos =info._pos;
	_menuPos = info._menuPos;
	_size = info._size;
	_type = info._type;
	_rect = IRect(_pos.x+1, _pos.y+1, _size.x-1, _size.y-1);
	_empty = true;
	_light = false;
	_pressed = false;
	_cost = info._cost;
	_hint = false;
	_curGold = 0;

};

void Button::Draw() {
	if (_tex) {
		_tex->Draw(_rect, _buttonUVRect); // Рисуем текстуру.
		
		if (_light && !_pressed) {
			Render::device.SetBlendMode(BlendMode::Add); // Включаем аддитивный блендинг.
			Render::BeginAlphaMul(0.5f); // Дальше всё будет полупрозрачное.
			_tex->Draw(_rect, _buttonUVRect); // “Прибавляем” текстуру саму к себе, что даёт эффект свечения.

			Render::EndAlphaMul();
			Render::device.SetBlendMode(BlendMode::Alpha); // Возвращаем нормальный блендинг.
		}
		
		if (_pressed) {
			Render::device.SetBlendMode(BlendMode::Add); // Включаем аддитивный блендинг.
			
			_tex->Draw(_rect, _buttonUVRect); // “Прибавляем” текстуру саму к себе, что даёт эффект свечения.
			
			
			Render::device.SetBlendMode(BlendMode::Alpha); // Возвращаем нормальный блендинг.
		}
		if (_cost > 0) {
			Render::BindFont("arial");
			if (_cost > _curGold) {
				Render::BeginColor(Color(255, 0, 0, 255));
				Render::PrintString(_rect.RightBottom(), utils::lexical_cast(_cost), 1.00f, RightAlign, BottomAlign);
				Render::EndColor();
			}
			else {
				Render::BeginColor(Color(255, 255, 255, 255));
				Render::PrintString(_rect.RightBottom(), utils::lexical_cast(_cost), 1.00f, RightAlign, BottomAlign);
				Render::EndColor();
			}
			
			
		}
		
		

	}
	else {
		Render::device.SetTexturing(false);
		Render::BeginColor(Color(255,255,255,255));
		Render::DrawRect(_rect);
		Render::EndColor();
		Render::device.SetTexturing(true);
		
	}
	
};
void Button::DrawHint() {
	if (_hint && _texHint) {
		IRect rect = IRect(_rect.x - 240, _rect.y - 32, 240, 128);
		
		FRect uv = FRect(0, 1, 0, 1);
		Render::BeginAlphaMul(0.85);
		_texHint->Draw(rect, uv);
		Render::EndAlphaMul();
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		switch (_type)
		{
		case NORMAL:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Normal tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y + 80, 230, "Simple low cost one target tower.", 1.0f, CenterAlign, TopAlign);
			
			break;
		case SPLASH:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Splash tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y +80, 230, "Powerful tower, damage all targets in a small area.", 1.0f, CenterAlign, TopAlign);

			break;
		case SLOW:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Slow tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y + 85, 230, "Low damage tower, decrease movespeed of all targets in a small area.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 45, 230, "Effective against healing monsters.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 30, 230, "Weak against immune monsters.", 1.0f, CenterAlign, TopAlign);
			break;
		case DECAY:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Poison tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y + 85, 230, "Low damage tower, poison target with strong toxin.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 45, 230, "Effective against boss monsters.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 30, 230, "Weak against healing monsters.", 1.0f, CenterAlign, TopAlign);
			break;
		case BASH:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Bash tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y + 85, 230, "Low damage tower, have a chance to stun target.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 45, 230, "Effective against immune monsters.", 1.0f, CenterAlign, TopAlign);
			Render::PrintString(rect.x + 120, rect.y + 30, 230, "Weak against boss monsters.", 1.0f, CenterAlign, TopAlign);
			break;
		case DESTROY:
			Render::PrintString(FPoint(rect.x + 120, rect.y + 105), "Destroy tower", 1.0f, CenterAlign, BottomAlign);
			Render::PrintString(rect.x + 120, rect.y + 80, 230, "Destroing tower and take some meat back.", 1.0f, CenterAlign, TopAlign);
			break;
		}
		Render::EndColor();

		
		//DrawHintText(rect);


	}
};

void Button::Update(float dt) {
};

void Button::SetLighter(IPoint mPos) {

	if (mPos.x>_pos.x && mPos.y>_pos.y && mPos.x<_pos.x+64 && mPos.y<_pos.y+64) {
		_light = true;
		_hint = true;

	}
	else {
		_light = false;
		_hint = false;
	}
};

TowerType Button::Press(IPoint mPos) {
	if (mPos.x>_pos.x && mPos.y>_pos.y && mPos.x<_pos.x + _size.x && mPos.y<_pos.y + _size.x) {
		_pressed = !_pressed;
		if (_pressed) {
			return _type;
		}
		else {
			return EMPTY;
		}
	}
	else {
		_pressed = false;
		return EMPTY;
	}
	
	
};
void Button::Reset() {
	_pressed = false;
	_light = false;
};

void Button::SetCurGold(int g) {
	_curGold = g;
};

Menu::Menu() {};
Menu::Menu(MenuInfo info, std::vector<TowerParent::Ptr> & towers) {

	_rect=info._rect;
	_buttonSize=info._buttonSize;
	_size=info._size;
	_buttonsTex=info._buttonsTex;
	int k = 0;

	for (int i = 0; i < 2; i++) {
		std::vector<Button::Ptr> vec;
		_buttons.push_back(vec);
		for (int j = 0; j < 3; j++) {
			Button::ButtonInfo info;
			info._tex = Core::resourceManager.Get<Render::Texture>("Towers");
			info._empty = false;
			info._menuPos = IPoint(i, j);
			info._pos = FPoint(_rect.LeftTop().x  + _buttonSize.x *(i), _rect.LeftTop().y - _buttonSize.y *(j + 1));
			info._size = _buttonSize;
			if ((i * 3 + j)* 1.0 / 6.0 + 1.0 / 6.0 <= 1) {
				info._buttonUVRect = FRect((i * 3 + j)*1.0/6.0, (i * 3 + j)*1.0 / 6.0 + 1.0 / 6.0, 0, 1.0);
			}
			else {
				info._buttonUVRect = FRect(0.8, 1.0, 0, 1.0);
			}
			
			info._type = static_cast<TowerType>(i * 3 + j);
			if (i * 3 + j < towers.size()) {
				info._cost = towers[i * 3 + j]->Price();
			}
			else {
				info._cost = 0;

			}
				
			Button::Ptr button = new Button(info);
			_buttons[i].push_back(button);
		}
		k++;
	}
};
Menu::~Menu() {};
TowerType Menu::Press(IPoint mPos, TowerType cur) {
	TowerType tmp = cur;
	bool allEmpty = true;
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			
			if (_rect.Contains(mPos)) {
				if (_buttons[i][j]) {
					tmp = _buttons[i][j]->Press(mPos);
					
					if (tmp != EMPTY ) {
						cur = tmp;
						allEmpty = false;
					}
				}
			}
			else {
				allEmpty = false;
			}
			
			
		}
	}
	if (allEmpty)
		cur = EMPTY;
	return cur;
};

void Menu::Draw() {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if(_buttons[i][j])
				_buttons[i][j]->Draw();
		}
	}
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if (_buttons[i][j])
				_buttons[i][j]->DrawHint();
		}
	}
};


void Menu::Update(float dt) {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if (_buttons[i][j])
				_buttons[i][j]->Update(dt);
		}
	}
};

void Menu::SetLighter(IPoint pos) {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if (_buttons[i][j])
				_buttons[i][j]->SetLighter(pos);
		}
	}
};
void Menu::Reset() {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if (_buttons[i][j])
				_buttons[i][j]->Reset();
		}
	}
};
void Menu::SetCurGold(int g) {
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			if (_buttons[i][j])
				_buttons[i][j]->SetCurGold(g);
		}
	}
};
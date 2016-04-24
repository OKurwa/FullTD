#include "stdafx.h"
#include "Hint.h"
#include "Menu.h"
using namespace rapidxml;
using namespace Xml;
using namespace std;
const int NO_VALUE = -1;
Button::Button() {
	_tex = nullptr;
	_rect = IRect(0,0,0,0);
	_buttonUVRect  = FRect(0,0,0,0);
	_pos = FPoint(0, 0);
	_menuPos = IPoint(0,0);
	_size = IPoint(0, 0);
	_value = NO_VALUE;
	_empty = true;
	_light = false;
	_pressed = false;

};

void Button::Draw() {
	if (_tex) {
		if (_enabled) {
			_tex->Draw(_rect, _buttonUVRect); // Рисуем текстуру.
		}else{
			Render::BeginColor(Color(255, 0, 0, 255));
			_tex->Draw(_rect, _buttonUVRect); // Рисуем текстуру.
			Render::EndColor();
		}
		
		if (_light && !_pressed) {
			Render::device.SetBlendMode(BlendMode::Add); // Включаем аддитивный блендинг.
			Render::BeginAlphaMul(0.5f); // Дальше всё будет полупрозрачное.
			
			//Render::BeginColor(Color(255, 0, 0, 255));
			_tex->Draw(_rect, _buttonUVRect); // “Прибавляем” текстуру саму к себе, что даёт эффект свечения.
			//Render::EndColor();
			Render::EndAlphaMul();
			Render::device.SetBlendMode(BlendMode::Alpha); // Возвращаем нормальный блендинг.
		}
		
		if (_pressed) {
			Render::device.SetBlendMode(BlendMode::Add); // Включаем аддитивный блендинг.
			
			_tex->Draw(_rect, _buttonUVRect); // “Прибавляем” текстуру саму к себе, что даёт эффект свечения.
			
			
			Render::device.SetBlendMode(BlendMode::Alpha); // Возвращаем нормальный блендинг.
		}
	}
	else {
		Render::device.SetTexturing(false);
		Render::BeginColor(Color(255,255,255,255));
		Render::DrawRect(_rect);
		Render::EndColor();
		Render::device.SetTexturing(true);
		
	}
	Render::BindFont("arial");
	Render::BeginColor(Color(255, 255, 255, 255));
	Render::PrintString(_pos.x + _size.x, _pos.y, _cornerText, 1.0f, RightAlign, BottomAlign);
	Render::EndColor();

};



void Button::DrawHint() {
	if (_hintEnable && _hint) {
		_hint->Draw();		
	}
};

void Button::Update(float dt) {
};

void Button::SetLighter(IPoint mPos) {

	if (mPos.x>_pos.x && mPos.y>_pos.y && mPos.x<_pos.x+ _size.x && mPos.y<_pos.y+ _size.y) {
		_light = true;
		_hintEnable = true;

	}
	else {
		_light = false;
		_hintEnable = false;
	}
};

int Button::Press(IPoint mPos) {
	if (mPos.x>_pos.x && mPos.y>_pos.y && mPos.x<_pos.x + _size.x && mPos.y<_pos.y + _size.y) {
		_pressed = !_pressed;
		if (_pressed) {
			return _value;
		}
		else {
			return NO_VALUE;
		}
	}
	else {
		_pressed = false;
		return NO_VALUE;
	}
	
	
};

void Button::Reset() {
	_pressed = false;
	_light = false;
};

void Button::SetCurGold(int g) {
	_curGold = g;
};

int Button::Value() {
	return _value;
};

void Button::LoadButtonFormXML(rapidxml::xml_node<>* buttonNode) {
	string value_t = buttonNode->first_attribute("texture")->value();
	_tex = Core::resourceManager.Get<Render::Texture>(value_t);
	float uStart = GetFloatAttributeOrDef(buttonNode, "uStart", 0);
	float uEnd = GetFloatAttributeOrDef(buttonNode, "uEnd", 1);
	float vStart = GetFloatAttributeOrDef(buttonNode, "vStart", 0);
	float vEnd = GetFloatAttributeOrDef(buttonNode, "vEnd", 1);
	
	_buttonUVRect = FRect(uStart,uEnd,vStart,vEnd);
	_value = GetIntAttributeOrDef(buttonNode, "id", NO_VALUE);
	_empty = true;
	_light = false;
	_pressed = false;
	_hintEnable = false;
	_enabled = true;
	_cornerText = GetStringAttributeOrDef(buttonNode, "cornerText", "");
	_hint = new Hint;
	_hint->LoadFromXml(buttonNode->first_node("Hint"));
};

void Button::SetRect(IRect rect) {
	_rect = rect;
	_pos = _rect.LeftBottom();
	_size = IPoint(_rect.Width(),_rect.Height());
	if (_hint) {
		IPoint hintPos = _pos.Rounded();
		hintPos.x += _rect.Width() / 2;
		if (hintPos.y - _hint->Height() < 0)
			hintPos.y += _rect.Height() + _hint->Height();
		_hint->SetPos(hintPos);
	
	}

	
};
void Button::SetMenuPos(IPoint menuPos) {
	_menuPos = menuPos;
};


void Button::SetEnabled(bool e) {
	_enabled = e;
};



Menu::Menu() {};
Menu::~Menu() {};

int Menu::Press(IPoint mPos, int cur) {
	int tmp = cur;
	bool allEmpty = true;
	for (unsigned int i = 0; i < _buttons.size(); i++) {
		for (unsigned int j = 0; j < _buttons[i].size(); j++) {
			
			if (_rect.Contains(mPos)) {
				if (_buttons[i][j]) {
					tmp = _buttons[i][j]->Press(mPos);
					
					if (tmp != NO_VALUE) {
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
		cur = NO_VALUE;
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


void Menu::LoadFromXml(std::string filename) {
	try {
		file<> file(filename.c_str());
		// Может бросить исключение, если нет файла.

		xml_document<> doc;
		doc.parse<0>(file.data());
		// Может бросить исключение, если xml испорчен.

		xml_node<>* menu = doc.first_node();
		if (!menu) { Assert(false); throw runtime_error("No root node"); }
		
		_rect.x = GetIntAttributeOrDef(menu, "posX", 0);
		_rect.y = GetIntAttributeOrDef(menu, "posY", 0);
		_rect.width = GetIntAttributeOrDef(menu, "width", 0);
		_rect.height = GetIntAttributeOrDef(menu, "height", 0);
		
		_buttonSize.x = GetIntAttributeOrDef(menu, "buttonWidth", 0);
		_buttonSize.y = GetIntAttributeOrDef(menu, "buttonHeight", 0);
		_size.x = GetIntAttributeOrDef(menu, "cols", 0);
		_size.y = GetIntAttributeOrDef(menu, "rows", 0);
		_buttons.resize(_size.x);
		for (int i = 0; i < _buttons.size(); i++) {
			_buttons[i].resize(_size.y);
		}

		vector<Button::Ptr> loadedButtons;

		for (xml_node<>* button = menu->first_node("Button"); button; button = button->next_sibling("Button")) {
			Button::Ptr tmpButton = new Button;
			tmpButton->LoadButtonFormXML(button);
			loadedButtons.push_back(tmpButton);
		};
		
		

		for (int i = 0; i < _buttons.size(); i++) {
			for (int j = 0; j < _buttons[i].size(); j++) {
				if ((i * _buttons[i].size() + j) < loadedButtons.size()) {
					FPoint pos = FPoint(_rect.LeftTop().x + _buttonSize.x *(i), _rect.LeftTop().y - _buttonSize.y *(j + 1));
					loadedButtons[i * _buttons[i].size() + j]->SetRect(IRect(pos.x + 1, pos.y + 1, _buttonSize.x - 1, _buttonSize.y - 1));
					loadedButtons[i * _buttons[i].size() + j]->SetMenuPos(IPoint(i, j));
					_buttons[i][j] = loadedButtons[i * _buttons[i].size() + j];
				}
			}
		}
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
};

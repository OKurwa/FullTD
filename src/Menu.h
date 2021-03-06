#pragma once
#include "TowerParent.h"
#include "Hint.h"

//������
class Button : public RefCounter
{
public:
	static const int NO_VALUE = -1;
	typedef boost::intrusive_ptr<Button> Ptr;
	Button();
	~Button() {};
	void Draw();
	void DrawHint();
	void Update(float dt);
	void SetLighter(IPoint);
	int Press(IPoint);
	void Reset();
	
	int Value();
	void LoadButtonFormXML(rapidxml::xml_node<>* buttonNode);
	void SetRect(IRect);
	void SetMenuPos(IPoint);
	void SetEnabled(bool);
	void MoveBy(IPoint);
	void SetCornerText(std::string);
private:
	Render::TexturePtr _tex;
	IRect _rect;
	FRect _buttonUVRect;
	FPoint _pos;
	IPoint _menuPos;
	IPoint _size;
	bool _light;
	bool _pressed;
	bool _empty;
	bool _enabled;
	std::string _cornerText;
	//int _curGold;
	bool _hintEnable;
	int _value;
	Hint::Ptr _hint;
};


//����
class Menu : public RefCounter {
public:
	
	struct MenuInfo {
		IRect _rect;
		IPoint _buttonSize;
		IPoint _size;

		Render::TexturePtr _buttonsTex;
		Render::TexturePtr _border;
		Render::TexturePtr _background;
		Render::TexturePtr _meat;
		Render::TexturePtr _time;
		Render::TexturePtr _wave;
	};
	typedef boost::intrusive_ptr<Menu> Ptr;
	Menu();
	~Menu();
	int Press(IPoint mPos, int cur);
	void Draw();
	void Update(float dt);
	void SetLighter(IPoint);
	void Reset();
	
	void LoadFromXml(std::string);
	void SetDisabled(std::vector<int>);
	void MoveTo(IPoint);
	std::vector<std::vector<Button::Ptr>> & Buttons();
private:
	IRect _rect;
	IPoint _buttonSize;
	IPoint _size;
	Render::TexturePtr _buttonsTex;
	std::vector<std::vector<Button::Ptr>> _buttons;
	

};
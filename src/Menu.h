#pragma once
#include "TowerParent.h"
//Кнопка
class Button : public RefCounter
{
public:
	struct ButtonInfo {
		Render::TexturePtr _tex;
		FRect _buttonUVRect;
		FPoint _pos;
		IPoint _menuPos;
		IPoint _size;
		TowerType _type;
		int _cost;
		bool _empty;
	};
	typedef boost::intrusive_ptr<Button> Ptr;
	Button();
	Button(ButtonInfo);
	~Button() {};
	void Draw();
	void DrawHint();
	void Update(float dt);
	void SetLighter(IPoint);
	TowerType Press(IPoint);
	void Reset();
	void SetCurGold(int);
private:
	Render::TexturePtr _tex;
	Render::TexturePtr _texHint;
	IRect _rect;
	FRect _buttonUVRect;
	FPoint _pos;
	IPoint _menuPos;
	IPoint _size;
	TowerType _type;
	bool _light;
	bool _pressed;
	bool _empty;
	int _cost;
	int _curGold;
	int _hint;
};


//Меню
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
	Menu(MenuInfo, std::vector<TowerParent::Ptr> & towers);
	~Menu();
	TowerType Press(IPoint mPos, TowerType cur);
	void Draw();
	void Update(float dt);
	void SetLighter(IPoint);
	void Reset();
	void SetCurGold(int);
private:
	IRect _rect;
	IPoint _buttonSize;
	IPoint _size;

	Render::TexturePtr _buttonsTex;
	std::vector<std::vector<Button::Ptr>> _buttons;
	Render::TexturePtr _border;
	Render::TexturePtr _background;
	Render::TexturePtr _meat;
	Render::TexturePtr _time;
	Render::TexturePtr _wave;

};
#pragma once
#include "World.h"
#include "Menu.h"
///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///

class StartWidget : public GUI::Widget
{
public:
	StartWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw();
	void Update(float dt);
	
	void AcceptMessage(const Message& message);
	
	bool MouseDown(const IPoint& mouse_pos);
	void MouseMove(const IPoint& mouse_pos);
	void MouseUp(const IPoint& mouse_pos);
private:
	void Init();

private:
	float _timer;
	float _scale;
	float _angle;
	
	Menu::Ptr _startMenu;

	Render::TexturePtr _start_w;
	Render::TexturePtr _lose_w;
	Render::TexturePtr _win_w;
	Render::TextPtr _StartTitlePtr;
	Render::TextPtr _StartTextPtr;
	Render::TextPtr _LoseTitlePtr;
	Render::TextPtr _LoseTextPtr;
	Render::TextPtr _WinTitlePtr;
	Render::TextPtr _WinTextPtr;
};

#pragma once
#include "World.h"
#include "Menu.h"
///
/// ������ - �������� ���������� ������� �� ������.
/// �� ������������ ����, � ����� ����� ��������� ������ �������.
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
	
};
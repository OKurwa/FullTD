#include "stdafx.h"
#include <fstream>
#include "PanelWidget.h"

const int NO_VALUE = -1;
const Color titleColor = Color(255, 255, 0, 255);



PanelWidget::PanelWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0)
	, _angle(0)
	, _scale(0.f)
{
	Init();
}

void PanelWidget::Init()
{
	_menu = new Menu;
	_menu->LoadFromXml("Menu.xml");
}

void PanelWidget::Draw()
{
	Render::BindFont("dikovina_12");
	World::WorldInfo info = World::Instance().GetInfo();
	if (World::Instance().State() == DELAY || World::Instance().State() == WAVE) {
		
		Render::device.SetTexturing(true);

		Render::BindFont("dikovina_16");
		Render::BeginColor(titleColor);
		Render::PrintString(FPoint(896, 680), "HUNGRY ANTS TD", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();

		Render::BindFont("dikovina_12");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(820, 600), "HP : " + utils::lexical_cast(info.HP), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(820, 580), "WAVE BEGINS IN : " + utils::lexical_cast(math::round(info.delayTimer)) + "sec", 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(820, 560), "WAVES REMAINING : " + utils::lexical_cast(info.attacksRemaining), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(896, 520), "CURRENT WAVE : " + utils::lexical_cast(info.curAttack), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 500), "Name : " + info.name, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 480), "Type: " + info.type, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 460), "Monster HP : " + utils::lexical_cast(info.monsterMaxHP), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 440), "Monster speed : " + utils::lexical_cast(info.monsterSpeed), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 420), "Meat per monster : " + utils::lexical_cast(info.monsterMeat), 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();


		Render::BindFont("dikovina_16");
		Render::BeginColor(titleColor);
		Render::PrintString(FPoint(896, 330), "MEAT", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();

		Render::BeginColor(Color(255, 255, 255, 255));
		Render::BindFont("dikovina_24");
		Render::PrintString(FPoint(896, 295), utils::lexical_cast(info.gold), 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		Render::device.SetTexturing(false);
	}
		
	_menu->Draw();
}

void PanelWidget::Update(float dt)
{
	_menu->Update(dt);
}



bool PanelWidget::MouseDown(const IPoint &mouse_pos)
{	
	int id = World::Instance().GetButtonId();
	id = _menu->Press(mouse_pos, id);
	if (id == NO_VALUE)
		_menu->Reset();
	World::Instance().SetTowerTypeById(id);
	
	return false;
}


void PanelWidget::MouseMove(const IPoint &mouse_pos)
{
	_menu->SetLighter(mouse_pos);
}


void PanelWidget::MouseUp(const IPoint &mouse_pos)
{
	
}


void PanelWidget::AcceptMessage(const Message& message)
{
	//
	// ������� ����� ���������� ��������� � �����������.
	//
	// ��������, ��� ����� ������� � ���������� �������� �� �������� ���� ����������
	// ��������� KeyPress � ����� ������� � ���������.
	//
	if (message.getPublisher() == "KeyPress") 
	{
		int code = utils::lexical_cast<int>(message.getData());
		if (code < 0)
		{
			//
			// ������������� �������� ���������� ��� ����������� ����� ������,
			// ��������, -VK_RETURN, -VK_ESCAPE � �.�.
			//

			
			return;
		}
		
		//
		// ������������� �������� ���������� ��� �������� ��������.
		//
		
		

			
	}
	else if (message.getPublisher() == "ChangeGold") {
		if (message.getIntegerParam() == 1) {
			_menu->SetDisabled(World::Instance().GetDisabled());
		}

	}
}





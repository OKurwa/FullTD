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
	//_menuBG = Core::resourceManager.Get<Render::Texture>("MenuBG");
}

void PanelWidget::Draw()
{
	
	Render::BindFont("dikovina_16");
	Render::BeginColor(titleColor);
	Render::PrintString(FPoint(896, 680), "%StartPanelTitle%", 1.00f, CenterAlign, BottomAlign);
	Render::EndColor();
	Render::BeginColor(titleColor);
	Render::PrintString(FPoint(896, 330), "%MEAT%", 1.00f, CenterAlign, BottomAlign);
	Render::EndColor();
	Render::BindFont("dikovina_12");
	World::WorldInfo info = World::Instance().GetInfo();

	if (World::Instance().State() == DELAY || World::Instance().State() == WAVE || World::Instance().State() == START) {
		
		Render::device.SetTexturing(true);

		

		
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::BindFont("dikovina_16");
		Render::PrintString(FPoint(896, 600), "%HP%" + utils::lexical_cast(info.HP), 1.00f, CenterAlign, BottomAlign);
		Render::BindFont("dikovina_12");
		Render::PrintString(FPoint(896, 580), "%WB%" + utils::lexical_cast(math::round(info.delayTimer)) + "%SEC%", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 560), "%WR%" + utils::lexical_cast(info.attacksRemaining), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 520), "%CW%" + utils::lexical_cast(info.curAttack), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 500), "%NAME%" + info.name, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 480), "%TYPE%" + info.type, 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 460), "%MHP%" + utils::lexical_cast(info.monsterMaxHP), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 440), "%MS%" + utils::lexical_cast(info.monsterSpeed), 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(896, 420), "%MPM%" + utils::lexical_cast(info.monsterMeat), 1.00f, CenterAlign, BottomAlign);
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
	// Виджету могут посылаться сообщения с параметрами.
	//
	// Например, при вводе символа с клавиатуры виджетам на активном слое посылается
	// сообщение KeyPress с кодом символа в параметре.
	//
	if (message.getPublisher() == "KeyPress") 
	{
		int code = utils::lexical_cast<int>(message.getData());
		if (code < 0)
		{
			//
			// Отрицательные значения посылаются для виртуальных кодов клавиш,
			// например, -VK_RETURN, -VK_ESCAPE и т.д.
			//

			
			return;
		}
		
		//
		// Положительные значения посылаются для символов алфавита.
		//
		
		

			
	}
	else if (message.getPublisher() == "ChangeGold") {
		if (message.getIntegerParam() == 1) {
			_menu->SetDisabled(World::Instance().GetDisabled());
		}

	}
}





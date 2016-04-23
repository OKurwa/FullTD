#include "stdafx.h"
#include <fstream>
#include "StartWidget.h"




StartWidget::StartWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0)
	, _angle(0)
	, _scale(0.f)
{
	Init();
}

void StartWidget::Init()
{
	_start_w = Core::resourceManager.Get<Render::Texture>("StartWindow");
	_lose_w = Core::resourceManager.Get<Render::Texture>("LoseWindow");
	_win_w = Core::resourceManager.Get<Render::Texture>("WinWindow");
}

void StartWidget::Draw()
{
	GameState state = World::Instance().State();
	switch (state)
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
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(state), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(384, 530), "HUNGRY ANTS TD", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 440), "Help big hungry ants to get some meat.", 1.00f, CenterAlign, BottomAlign);
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
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(state), 1.00f, LeftAlign, BottomAlign);
		Render::EndColor();
		_win_w->Draw(FPoint(110, 180));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(state), 1.00f, LeftAlign, BottomAlign);
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
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(state), 1.00f, LeftAlign, BottomAlign);
		Render::EndColor();
		_lose_w->Draw(FPoint(110, 180));
		Render::BindFont("arial");
		Render::BeginColor(Color(255, 255, 255, 255));
		Render::PrintString(FPoint(50, 750), utils::lexical_cast(state), 1.00f, LeftAlign, BottomAlign);
		Render::PrintString(FPoint(384, 530), "DEFEAT!", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 440), "Now your ants became small and vegan!", 1.00f, CenterAlign, BottomAlign);
		Render::PrintString(FPoint(384, 390), "Press 's' to make them hunt again!", 1.00f, CenterAlign, BottomAlign);
		Render::EndColor();
		break;
	}







	
	
}

void StartWidget::Update(float dt)
{
	//anim->Update(dt*0.5);
	if (World::Instance().State() == WIN || World::Instance().State() == LOSE || World::Instance().State() == START)
		dt *= 0;
	if (World::Instance().State() == WAVE || World::Instance().State() == DELAY) {
		World::Instance().Update(dt);
	}
}



bool StartWidget::MouseDown(const IPoint &mouse_pos)
{	
	
	
	return false;
}


void StartWidget::MouseMove(const IPoint &mouse_pos)
{
	
}


void StartWidget::MouseUp(const IPoint &mouse_pos)
{
	
}


void StartWidget::AcceptMessage(const Message& message)
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
		if (code == 'q')
		{
			Core::appInstance->CloseWindow();
		}
		if (code == 'a')
		{
		}

		if (code == 's' || code == 'S') {
			Message msg = Message("StartLayer", 1);
			msg.SetTargetLayer("TestLayer");
			Core::mainScreen.ProcessMessage(msg);
			Core::mainScreen.popLayer();
			
		}
		
		
	}
}





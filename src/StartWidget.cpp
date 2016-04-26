#include "stdafx.h"
#include <fstream>
#include "StartWidget.h"

const Color titleColor = Color(255, 255, 0, 255);


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
	_startMenu = new Menu;
	_startMenu->LoadFromXml("StartMenu.xml");
	_startTitlePtr = Core::resourceManager.Get<Render::Text>("StartPanelTitle");
	_startTextPtr = Core::resourceManager.Get<Render::Text>("StartPanelText");
	_loseTitlePtr = Core::resourceManager.Get<Render::Text>("LosePanelTitle");
	_loseTextPtr = Core::resourceManager.Get<Render::Text>("LosePanelText");
	_winTitlePtr = Core::resourceManager.Get<Render::Text>("WinPanelTitle");
	_winTextPtr = Core::resourceManager.Get<Render::Text>("WinPanelText");
}

void StartWidget::Draw()
{
	Render::BindFont("dikovina_12");
	GameState state = World::Instance().State();
	Render::TextPtr _TitlePtr;
	Render::TextPtr _TextPtr;
	
	Render::device.SetTexturing(false);
	Render::BeginAlphaMul(0.5);
	Render::BeginColor(Color(0, 0, 0, 255));
	Render::DrawRect(0, 0, Render::device.Width(), Render::device.Height());
	Render::EndColor();
	Render::EndAlphaMul();
	switch (state)
	{
	case START:
		Render::device.SetTexturing(true);
		_start_w->Draw(FPoint(110, 180));
		_TitlePtr = _startTitlePtr;
		_TextPtr = _startTextPtr;

		break;
	case WIN:
		_win_w->Draw(FPoint(110, 180));
		_TitlePtr = _winTitlePtr;
		_TextPtr =_winTextPtr;

		break;
	case LOSE:
		_lose_w->Draw(FPoint(110, 180));
		_TitlePtr = _loseTitlePtr;
		_TextPtr = _loseTextPtr;
		break;
	}

	if (_TitlePtr) {
		std::string str = _TitlePtr->ToString();
		str = "{font size=\"16\"}" + str + "{}";
		Render::PrintString(FPoint(384, 525), str, 1.00f, CenterAlign, BottomAlign);
	}
		
	if(_TextPtr)
		Render::PrintString(FPoint(384, 390), _TextPtr->ToString(), 1.00f, CenterAlign, BottomAlign);
	_startMenu->Draw();
}

void StartWidget::Update(float dt)
{
	//anim->Update(dt*0.5);
	if (World::Instance().State() == WAVE || World::Instance().State() == DELAY) {
		World::Instance().Update(dt);
	}
}



bool StartWidget::MouseDown(const IPoint &mouse_pos)
{	
	int id = _startMenu->Press(mouse_pos, Button::NO_VALUE);
	_startMenu->Reset();
	if (id == 1) {
		Core::appInstance->CloseWindow();
	}
	else if (id == 0) {
		Message msg = Message("StartLayer", 1);
		msg.SetTargetLayer("TestLayer");
		Core::mainScreen.ProcessMessage(msg);
		Core::mainScreen.popLayer();
	}
	return false;
}


void StartWidget::MouseMove(const IPoint &mouse_pos)
{
	_startMenu->SetLighter(mouse_pos);
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

		
		
		
	}
}





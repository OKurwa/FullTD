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
}

void StartWidget::Draw()
{
	
	World::Instance().Draw();
	
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





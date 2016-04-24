#include "stdafx.h"
#include "Hint.h"
using namespace rapidxml;
using namespace std;

void Hint::LoadFromXml(xml_node<>* hintNode)
{
	string value_t = hintNode->first_attribute("texture")->value();
	_hintTex = Core::resourceManager.Get<Render::Texture>(value_t);
	_rect.height = Xml::GetIntAttributeOrDef(hintNode, "height", 0);
	_rect.width = Xml::GetIntAttributeOrDef(hintNode, "width", 0);
	xml_node<>* titleNode = hintNode->first_node("HintTitle");
	if (titleNode) {
		_title = Xml::GetStringAttributeOrDef(titleNode, "text", "");
		_titleYPos = Xml::GetIntAttributeOrDef(titleNode, "down", 0);
	}
	_lines.clear();
	for (xml_node<>* textNode = hintNode->first_node("HintText"); textNode; textNode = textNode->next_sibling("HintText")) {
		hintString line;
		line._text = Xml::GetStringAttributeOrDef(textNode, "text", "");
		line._lineYPos = Xml::GetIntAttributeOrDef(textNode, "down", 0);
		_lines.push_back(line);
	}

	
}

void Hint::SetPos(IPoint pos)
{
	_position = pos;
	_rect.MoveTo(IPoint(_position.x - _rect.Width()/2, _position.y - _rect.Height()));
	if (_rect.x < 0)
		_rect.x = 0;
	if (_rect.x + _rect.Width() > Render::device.Width())
		_rect.x = Render::device.Width() - _rect.Width();
	if (_rect.y < 0)
		_rect.y = 0;
	if (_rect.y + _rect.Height() > Render::device.Height())
		_rect.y = Render::device.Width() - _rect.Height();

	
}

void Hint::Draw()
{
	if (_hintTex) {
		Render::BeginAlphaMul(0.85);
		_hintTex->Draw(_rect, FRect(0, 1, 0, 1));
		Render::EndAlphaMul();
		
		Render::PrintString(_rect.x + _rect.Width()/2, _rect.y + _rect.Height() - _titleYPos, _title, 1.0f, CenterAlign, TopAlign);
		
		for (unsigned int i = 0; i < _lines.size(); i++) {
			Render::PrintString(_rect.x + _rect.Width() / 2, _rect.y + _rect.Height() - _lines[i]._lineYPos, _rect.Width() - 30, _lines[i]._text, 1.0f, CenterAlign, TopAlign);
		}
		
		
	}
}

int Hint::Height()
{
	return _rect.Height();
}

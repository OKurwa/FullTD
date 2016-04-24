#pragma once

//Класс подсказки
class Hint : public RefCounter{
public:
	typedef boost::intrusive_ptr<Hint> Ptr;
	Hint() {};
	~Hint() {};
	void LoadFromXml(rapidxml::xml_node<>* hintNode);
	void SetPos(IPoint pos);
	void Draw();
	int Height();
private:
	IPoint _position;
	IRect _rect;
	Render::TexturePtr _hintTex;
	std::string _title;
	int _titleYPos;
	struct hintString{
		int _lineYPos;
		std::string _text;
	};
	std::vector<hintString> _lines;
	
};
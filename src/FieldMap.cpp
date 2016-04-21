#include "stdafx.h"
#include "FieldMap.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace rapidxml;

FieldCell::FieldCell() : ref_cnt_(0)
{
	_cellType = NONE;
	_selected = false;
	_size = IPoint(0,0);
	_position = FPoint(0,0);
	_showGhost = false;
}

FieldCell::~FieldCell()
{
}


void FieldCell::Init(CellType cell, IPoint size, FPoint position) {
	_cellType = cell;
	_tex = Core::resourceManager.Get<Render::Texture>("Land");
	_ghost = Core::resourceManager.Get<Render::Texture>("Ghost");
	_size = size;
	_position = position;
	_selected = false;
	
};



void	FieldCell::Draw(Render::Texture* _ground, FRect cTexRect) {
	if (_tex) {
		IRect cRect = IRect(_position.x * _size.x, _position.y * _size.y, _size.x, _size.y);
		FRect tmp = FRect(0, 1, 0, 1);


		_tex->SetAddressMode(AddressMode::Clamp);

		Render::device.SetTexturing(true);
		_tex->Draw(cRect, cTexRect);
		if (_ghost && _cellType == SLOT && _empty && _showGhost) {
			Render::BeginAlphaMul(0.5);
			_ghost->Draw(cRect, tmp);
			Render::EndAlphaMul();
		}
		Render::device.SetTexturing(false);		

	}
	
		
};

void FieldCell::Draw() {
	FRect cTexRect;
	FRect tmp = FRect(0, 1, 0, 1);
	Color c;


	if (_tex) {

		IRect r=IRect(_position.x * _size.x, _position.y * _size.y, 64, 64);
		
		Render::device.SetTexturing(true);
		_tex->Draw(r, tmp);
		if (_ghost && _cellType == SLOT && _empty && _showGhost) {
			_ghost->Draw(r, tmp);
		}
		Render::device.SetTexturing(false);
	}
	else {
		switch (_cellType)
		{
		case PASS:
			c = Color(200, 0, 0, 255);
			break;
		case SLOT:
			c = Color(200, 200, 200, 255);
			break;
		case GRASS:
			c = Color(100, 100, 100, 255);
			break;
		case BORDER_0:
			c = Color(150, 100, 100, 255);
			break;
		case BORDER_90:
			c = Color(150, 100, 100, 255);
			break;
		case BORDER_180:
			c = Color(150, 100, 100, 255);
			break;
		case BORDER_270:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_IN_0:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_IN_90:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_IN_180:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_IN_270:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_OUT_0:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_OUT_90:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_OUT_180:
			c = Color(150, 100, 100, 255);
			break;
		case CORNER_OUT_270:
			c = Color(150, 100, 100, 255);
			break;
		case SPAWN:
			c = Color(200, 200, 200, 255);
			break;
		case LAIR:
			c = Color(100, 100, 200, 255);
			break;
		default:
			cTexRect = FRect((7.0 / 8.0), (8.0 / 8.0), 0.0, (1.0 / 3.0));
			break;
		}
		//_cells[row][col]->Draw(_ground, tmp);
		if (_selected) {
			c = c = Color(100, 100, 255, 255);
		}
		IRect cRect = IRect(_position.x * _size.x + 1, _position.y * _size.y + 1, _size.x - 1, _size.y - 1);
		//Render::device.SetTexturing(false);
		Render::BeginColor(c);
		Render::DrawRect(cRect);
		Render::EndColor();
		//Render::device.SetTexturing(true);

	}
	

};

void	FieldCell::Update(float dt) {
		
};


CellType FieldCell::Type() {
	return _cellType;
};

IPoint FieldCell::Size() {
	return _size;
};

FPoint FieldCell::Position() {
	return _position;
};

bool FieldCell::Selected() {
	return _selected;
};

bool FieldCell::Empty() {
	return _empty;
};

void FieldCell::SetPos(FPoint pos) {
	_position = pos;
};

void FieldCell::SetSize(IPoint size) {
	_size = size;
};


void FieldCell::SetType(CellType cell) {
	_cellType = cell;
	if (cell == SLOT) {
		_tex = Core::resourceManager.Get<Render::Texture>("Slot");
		
	}
	else {
		_tex = nullptr;
	}
};

void FieldCell::Select() {
	_selected = true;
};

void FieldCell::UnSelect() {
	_selected = false;
};

bool FieldCell::AddTower() {
	if (_cellType == SLOT && _empty) {
		_empty = false;
		return true;
	}
	else {
		return false;
	}
};

bool FieldCell::DestroyTower() {
	if (_cellType == SLOT && !_empty) {
		_empty = true;
		return true;
	}
	else {
		return false;
	}
};

void FieldCell::ShowGhosts(bool sG) {
	if (sG) {
		_showGhost = true;
	}
	else {
		_showGhost = false;
	}
};





FieldMap::FieldMap() {
	_size = IPoint(0,0);
	_cellSize = IPoint(0, 0);
	_ground = nullptr;
	_showGhosts = false;
};
FieldMap::~FieldMap() {
	for (unsigned int i = 0; i < _cells.size(); i++) {
		_cells[i].clear();
	}
	_cells.clear();
};
/*
FieldMap::FieldMap(const FieldMap& map) {
	_size = map._size;
	_cells = map._cells;
};
FieldMap& FieldMap::operator = (const FieldMap& map) {
	if (this == &map) return *this;
	_size = map._size;
	_cells = map._cells;
	return *this;
};
*/

void FieldMap::Init() {
	for (int row = 0; row < _size.x; row++) {
		std::vector<boost::intrusive_ptr<FieldCell>> nCol;
			_cells.push_back(nCol);
		for (int col = 0; col < _size.x; col++) {
			boost::intrusive_ptr<FieldCell> cell(new FieldCell);
			_cells[row].push_back(cell);
		}
	}
	//_ground = Core::resourceManager.Get<Render::Texture>(tex);
};

void FieldMap::TryInit() {
	//_ground = Core::resourceManager.Get<Render::Texture>("TestGrass");
	_size = IPoint(12,12);
	_cellSize = IPoint(64, 64);
	for (int row = 0; row < _size.x; row++) {
		std::vector<boost::intrusive_ptr<FieldCell>> nCol;
		_cells.push_back(nCol);
		for (int col = 0; col < _size.y; col++) {
			boost::intrusive_ptr<FieldCell> cell(new FieldCell);
			FPoint cPos = FPoint(row,col);
			if (col == 5 || col == 6) {
				cell->Init(PASS, _cellSize, cPos);
			}
			else if (col == 4) {
				cell->Init(BORDER_0, _cellSize, cPos);
			}
			else if (col == 7) {
				cell->Init(BORDER_180, _cellSize, cPos);
			}
			else if (col == 3|| col == 8) {
				cell->Init(SLOT, _cellSize, cPos);
			}
			else {
				cell->Init(GRASS, _cellSize, cPos);
			}
				
			
			_cells[row].push_back(cell);
		}
	}
};

void FieldMap::Draw() {
	//Render::device.SetTexturing(false);
	for (unsigned int row = 0; row < _cells.size(); row++) {
		for (unsigned int col = 0; col < _cells[row].size(); col++) {
			FRect cTexRect;
			FRect tmp = FRect(0, 1, 0, 1);
			switch (_cells[row][col]->Type())
				{
				case PASS:
					cTexRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case SLOT:
					//cTexRect = FRect((0.0 / 8.0), (1.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					cTexRect = FRect((4.0 / 8.0), (5.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case GRASS:
					cTexRect = FRect((4.0 / 8.0), (5.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case BORDER_0:
					cTexRect = FRect((4.0 / 8.0), (5.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case BORDER_90:
					cTexRect = FRect((3.0 / 8.0), (4.0 / 8.0), (0.0 / 3.0), (1.0 / 3.0));
					break;
				case BORDER_180:
					cTexRect = FRect((0.0 / 8.0), (1.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case BORDER_270:
					cTexRect = FRect((5.0 / 8.0), (6.0 / 8.0), (0.0 / 3.0), (1.0 / 3.0));
					break;
				case CORNER_IN_0:
					cTexRect = FRect((2.0 / 8.0), (3.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case CORNER_IN_90:
					cTexRect = FRect((3.0 / 8.0), (4.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case CORNER_IN_180:
					cTexRect = FRect((3.0 / 8.0), (4.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case CORNER_IN_270:
					cTexRect = FRect((2.0 / 8.0), (3.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case CORNER_OUT_0:
					cTexRect = FRect((5.0 / 8.0), (6.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case CORNER_OUT_90:
					cTexRect = FRect((6.0 / 8.0), (7.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case CORNER_OUT_180:
					
					cTexRect = FRect((6.0 / 8.0), (7.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case CORNER_OUT_270:
					cTexRect = FRect((5.0 / 8.0), (6.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
					break;
				case LAIR:
					cTexRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				case SPAWN:
					cTexRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
					break;
				default:
					cTexRect = FRect((7.0 / 8.0), (8.0 / 8.0), 0.0, (1.0 / 3.0));
					break;
			}
			_cells[row][col]->Draw(nullptr, cTexRect);
			//_cells[row][col]->Draw();
		}
	}
	//Render::device.SetTexturing(true);
};
void FieldMap::Update(float dt) {
	for (unsigned int row = 0; row < _cells.size(); row++) {
		for (unsigned int col = 0; col < _cells[row].size(); col++) {
			_cells[row][col]->ShowGhosts(_showGhosts);
			_cells[row][col]->Update(dt);

		}
	}

};

bool FieldMap::AddTower(IPoint cell) {
	if (cell.x >= 0 && cell.x < _size.x && cell.y >= 0 && cell.y < _size.y) {
		return _cells[cell.x][cell.y]->AddTower();
	}
	else {
		return false;
	}
	
};

bool FieldMap::DestroyTower(IPoint cell) {
	if (cell.x >= 0 && cell.x < _size.x && cell.y >= 0 && cell.y < _size.y) {
		return _cells[cell.x][cell.y]->DestroyTower();
	}
	else {
		return false;
	}
};


IPoint FieldMap::Size() {
	return _size;
};
IPoint FieldMap::CellSize() {
	return _cellSize;
};

CellType FieldMap::PosCellType(IPoint pos) {
	int x = pos.x / _cellSize.x;
	int y = pos.y / _cellSize.y;
	if (x >= 0 && x<_size.x && y >= 0 && y<_size.y) {
		return _cells[x][y]->Type();
	}
	else {
		return NONE;
	}
		
};

IPoint FieldMap::PosCell(FPoint pos) {
	int x = pos.x / _cellSize.x;
	int y = pos.y / _cellSize.y;
	if (x>=0 && x<_size.x && y >= 0 && y<_size.y) {
		return IPoint(x,y);
	}
	else{
		return IPoint(-1, -1);
	}

};

CellType FieldMap::SelectCell(FPoint pos) {
	int x = pos.x / _cellSize.x;
	int y = pos.y / _cellSize.y;
	for (unsigned int i = 0; i < _cells.size();i++) {
		for (unsigned int j = 0; j < _cells[i].size(); j++) {
			if (_cells[i][j]->Selected()) {
				_cells[i][j]->UnSelect();
			}
		}
	}
	if (x >= 0 && x<_size.x && y >= 0 && y<_size.y) {
		_cells[x][y]->Select();
		return _cells[x][y]->Type();
	}
	else {
		return NONE;
	}
	
};

void FieldMap::ShowGhosts(TowerType t) {
	if (t != EMPTY ) {
		_showGhosts = true;
	}
	else {
		_showGhosts = false;
	}
};


void FieldMap::Reset() {
	for (unsigned int i = 0; i < _cells.size(); i++) {
		for (unsigned int j = 0; j < _cells[i].size(); j++) {
			_cells[i][j]->DestroyTower();
		}
	}
};

std::vector<std::vector<boost::intrusive_ptr<FieldCell>>> FieldMap::Cells() {
	return _cells;
};

void FieldMap::LoadFromFile(std::string file) {
	for (unsigned int i = 0; i < _cells.size(); i++) {
		_cells[i].clear();
	}
	_cells.clear();
	_cellSize = IPoint(64, 64);
	std::ifstream settingsFile(file);
	std::string line;
	std::vector<std::string> lines;
	lines.clear();
	
	if (settingsFile.is_open()) {
		while (std::getline(settingsFile, line)) {
			if(line!="")
				lines.push_back(line);
			
		}
	}
	settingsFile.close();

	
	int row = 0;
	int col = 0;
	std::string name;
	std::string value;
	line = lines[0];
	size_t pos = line.find("=", 0);
	name = line.substr(0, pos);
	value = line.substr(pos + 1);
	if (name == "TYPE" && value == "TDMAP") {
		for (unsigned int i = 1; i < lines.size(); i++) {
			line = lines[i];
			size_t pos = line.find("=", 0);
			name = line.substr(0, pos);
			value = line.substr(pos + 1);
			
			if (name == "sizeX") {
				_size.x = utils::lexical_cast<int>(value);
			}

			if (name == "sizeY") {
				_size.y = utils::lexical_cast<int>(value);
			}

			if (name == "row") {
				row = utils::lexical_cast<int>(value);
				std::vector<boost::intrusive_ptr<FieldCell>> nCol;
				_cells.push_back(nCol);
				
			}

			if (name == "col") {
				col = utils::lexical_cast<int>(value);
				boost::intrusive_ptr<FieldCell> cell(new FieldCell);
				_cells[row].push_back(cell);
			}

			if (name == "celltype") {

				_cells[row][col]->SetType(static_cast<CellType>(utils::lexical_cast<int>(value)));
				_cells[row][col]->SetPos(FPoint(row, col));
				_cells[row][col]->SetSize(_cellSize);
				//_cells[row][col]->SetTexture("TestGrassIso");
			}

		}
	}
	else {
		TryInit();
	}



	
};

void FieldMap::LoadFromXml(std::string filename) {
	
	for (unsigned int i = 0; i < _cells.size(); i++) {
		_cells[i].clear();
	}
	_cells.clear();
	
	try {
		file<> file(filename.c_str());
		// Может бросить исключение, если нет файла.

		xml_document<> doc;
		doc.parse<0>(file.data());
		// Может бросить исключение, если xml испорчен.
		//xml_node<>*
		//xml_attribute<>*
		//boost::intrusive_ptr<xml_attribute<>>
		//boost::intrusive_ptr<xml_attribute<>>

		xml_node<>* game = doc.first_node();
		if (!game) { Assert(false); throw runtime_error("No root node"); }

		xml_node<>* map = game->first_node("Map");
		
		for (xml_attribute<>* attr = map->first_attribute(); attr; attr = attr->next_attribute()){
			if (utils::lexical_cast(attr->name()) == "rowCount") {
				_size.x = utils::lexical_cast<int>(attr->value());
				_cells.resize(_size.x);
				
			}
			if (utils::lexical_cast(attr->name()) == "colCount") {
				_size.y = utils::lexical_cast<int>(attr->value());
				for (unsigned int i = 0; i < _cells.size(); i++) {
					_cells[i].resize(_size.y);
				}
				
			}
			if (utils::lexical_cast(attr->name()) == "cellWidth") {
				_cellSize.x = utils::lexical_cast<int>(attr->value());
			}
			if (utils::lexical_cast(attr->name()) == "cellHeight") {
				_cellSize.y = utils::lexical_cast<int>(attr->value());
			}
		}

		for (xml_node<>* row = map->first_node("Row"); row; row = row->next_sibling("Row")) {
			int i = utils::lexical_cast<int>(row->first_attribute("index")->value());
			for (xml_node<>* col = row->first_node("Col"); col; col = col->next_sibling("Col")) {
				int j = utils::lexical_cast<int>(col->first_attribute("index")->value());
				string cType = utils::lexical_cast(col->first_attribute("type")->value());
				
				if (i < _size.x && j < _size.y) {
					if (cType == "PASS") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(PASS, _cellSize, FPoint(i, j));

					}
					else if (cType == "SLOT") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(SLOT, _cellSize, FPoint(i, j));

					}
					else if (cType == "GRASS") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(GRASS, _cellSize, FPoint(i, j));

					}
					else if (cType == "BORDER_0") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(BORDER_0, _cellSize, FPoint(i, j));

					}
					else if (cType == "BORDER_90") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(BORDER_90, _cellSize, FPoint(i, j));

					}
					else if (cType == "BORDER_180") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(BORDER_180, _cellSize, FPoint(i, j));

					}
					else if (cType == "BORDER_270") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(BORDER_270, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_IN_0") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_IN_0, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_IN_90") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_IN_90, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_IN_180") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_IN_180, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_IN_270") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_IN_270, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_OUT_0") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_OUT_0, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_OUT_90") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_OUT_90, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_OUT_180") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_OUT_180, _cellSize, FPoint(i, j));

					}
					else if (cType == "CORNER_OUT_270") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(CORNER_OUT_270, _cellSize, FPoint(i, j));

					}
					else if (cType == "SPAWN") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(SPAWN, _cellSize, FPoint(i, j));

					}
					else if (cType == "LAIR") {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(LAIR, _cellSize, FPoint(i, j));

					}
					else {
						_cells[i][j] = new FieldCell();
						_cells[i][j]->Init(NONE, _cellSize, FPoint(i, j));

					}
				}
					
			}
		}


	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}



};







void FieldMap::SaveToFile(std::string file) {
	_cellSize = IPoint(32, 32);
	std::ofstream settingsFile(file);
	std::string line;
	std::vector<std::string> lines;
	lines.clear();
	lines.push_back("TYPE=TDMAP");
	lines.push_back("");
	lines.push_back("sizeX=" + utils::lexical_cast(_size.x));
	lines.push_back("sizeY=" + utils::lexical_cast(_size.y));
	lines.push_back("");
	for (unsigned int row = 0; row < _size.x; row++) {
		lines.push_back("row=" + utils::lexical_cast(row));
		lines.push_back("");
		for (unsigned int col = 0; col < _size.y; col++) {
			lines.push_back("col=" + utils::lexical_cast(col));
			lines.push_back("celltype=" + utils::lexical_cast(static_cast<int>(_cells[row][col]->Type())));
			lines.push_back("");
		}
	}
	
	if (settingsFile.is_open()) {
		for (unsigned int i = 0; i < lines.size(); i++) {
			settingsFile << lines[i] << endl;
		}
	}
	settingsFile.close();


	
};



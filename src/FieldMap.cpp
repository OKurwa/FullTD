#include "stdafx.h"
#include "FieldMap.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace rapidxml;

FieldCell::FieldCell()
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
	switch (_cellType)
	{
	case PASS:
		_uvRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case SLOT:
		_uvRect = FRect((4.0 / 8.0), (5.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case GRASS:
		_uvRect = FRect((4.0 / 8.0), (5.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case BORDER_0:
		_uvRect = FRect((4.0 / 8.0), (5.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case BORDER_90:
		_uvRect = FRect((3.0 / 8.0), (4.0 / 8.0), (0.0 / 3.0), (1.0 / 3.0));
		break;
	case BORDER_180:
		_uvRect = FRect((0.0 / 8.0), (1.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case BORDER_270:
		_uvRect = FRect((5.0 / 8.0), (6.0 / 8.0), (0.0 / 3.0), (1.0 / 3.0));
		break;
	case CORNER_IN_0:
		_uvRect = FRect((2.0 / 8.0), (3.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case CORNER_IN_90:
		_uvRect = FRect((3.0 / 8.0), (4.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case CORNER_IN_180:
		_uvRect = FRect((3.0 / 8.0), (4.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case CORNER_IN_270:
		_uvRect = FRect((2.0 / 8.0), (3.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case CORNER_OUT_0:
		_uvRect = FRect((5.0 / 8.0), (6.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case CORNER_OUT_90:
		_uvRect = FRect((6.0 / 8.0), (7.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case CORNER_OUT_180:

		_uvRect = FRect((6.0 / 8.0), (7.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case CORNER_OUT_270:
		_uvRect = FRect((5.0 / 8.0), (6.0 / 8.0), (2.0 / 3.0), (3.0 / 3.0));
		break;
	case LAIR:
		_uvRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	case SPAWN:
		_uvRect = FRect((1.0 / 8.0), (2.0 / 8.0), (1.0 / 3.0), (2.0 / 3.0));
		break;
	default:
		_uvRect = FRect((7.0 / 8.0), (8.0 / 8.0), 0.0, (1.0 / 3.0));
		break;
	}
	
};



void	FieldCell::Draw() {
	if (_tex) {
		IRect cRect = IRect(_position.x * _size.x, _position.y * _size.y, _size.x, _size.y);
		FRect tmp = FRect(0, 1, 0, 1);
		Render::device.SetTexturing(true);
		_tex->Draw(cRect, _uvRect);
		if (_ghost && _cellType == SLOT && _empty && _showGhost) {
			Render::BeginAlphaMul(0.5);
			_ghost->Draw(cRect, tmp);
			Render::EndAlphaMul();
		}
		Render::device.SetTexturing(false);		

	}
	
		
};

CellType FieldCell::Type() {
	return _cellType;
};


bool FieldCell::Selected() {
	return _selected;
};

bool FieldCell::Empty() {
	return _empty;
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




void FieldMap::Draw() {
	for (unsigned int row = 0; row < _cells.size(); row++) {
		for (unsigned int col = 0; col < _cells[row].size(); col++) {
			_cells[row][col]->Draw();
		}
	}
	
};
void FieldMap::Update(float dt) {
	for (unsigned int row = 0; row < _cells.size(); row++) {
		for (unsigned int col = 0; col < _cells[row].size(); col++) {
			_cells[row][col]->ShowGhosts(_showGhosts);
		}
	}

};

bool FieldMap::AddTower(IPoint cell) {
	if (Contain(cell)) {
		return _cells[cell.x][cell.y]->AddTower();
	}
	else {
		return false;
	}
	
};

bool FieldMap::DestroyTower(IPoint cell) {
	if (Contain(cell)) {
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
	if (Contain(IPoint(x,y))) {
		return _cells[x][y]->Type();
	}
	else {
		return NONE;
	}
		
};

IPoint FieldMap::PosCell(FPoint pos) {
	int x = pos.x / _cellSize.x;
	int y = pos.y / _cellSize.y;
	if (Contain(IPoint(x, y))) {
		return IPoint(x,y);
	}
	else{
		return IPoint(-1, -1);
	}

};
bool FieldMap::Contain(IPoint point) {
	if (point.x >= 0 && point.x<_size.x && point.y >= 0 && point.y<_size.y) {
		return true;
	}
	else {
		return false;
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
	if (Contain(IPoint(x, y))) {
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
}
IPoint FieldMap::GetSpawnCell()
{

	for (unsigned int i = 0; i < _cells.size(); i++) {
		for (unsigned int j = 0; j < _cells.size(); j++) {
			if (_cells[i][j]->Type() == SPAWN) {
				return IPoint(i,j);
			}
		}
	}
	return IPoint(0,0);
}
;



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


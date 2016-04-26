#pragma once

enum CellType
{
	PASS,
	SLOT,
	GRASS,
	BORDER_0,
	BORDER_90,
	BORDER_180,
	BORDER_270,
	CORNER_IN_0,
	CORNER_IN_90,
	CORNER_IN_180,
	CORNER_IN_270,
	CORNER_OUT_0,
	CORNER_OUT_90,
	CORNER_OUT_180,
	CORNER_OUT_270,
	SPAWN,
	LAIR,
	NONE
};

class FieldCell : public RefCounter {
public:
	typedef boost::intrusive_ptr<FieldCell> Ptr;
	FieldCell();
	~FieldCell();
	
	
	void Init(CellType cell, IPoint size, FPoint position);
	void Draw();
	CellType Type();
	
	bool Selected();
	
	void Select();
	void UnSelect();
	bool AddTower();
	bool DestroyTower();
	bool Empty();
	
	
	void ShowGhosts(bool);
private:
	
	bool _selected;
	bool _empty;
	CellType _cellType;
	IPoint _size;
	FPoint _position;
	FRect  _uvRect;
	
	
	Render::TexturePtr _tex;
	Render::TexturePtr _ghost;
	bool _showGhost;

};






class FieldMap
{
public:
	FieldMap() ;
	~FieldMap();
	
	void LoadFromXml(std::string &);
	void Draw();
	void Update(float dt);
	bool AddTower(IPoint);
	bool DestroyTower(IPoint);
	IPoint Size();
	IPoint CellSize();
	CellType PosCellType(IPoint);
	IPoint PosCell(FPoint pos);
	bool Contain(IPoint);
	CellType SelectCell(FPoint pos);
	void ShowGhosts(TowerType);
	void Reset();
	std::vector<std::vector<FieldCell::Ptr>> Cells();
	IPoint GetSpawnCell();
private:
	
	IPoint _size;
	IPoint _cellSize;
	std::vector<std::vector<FieldCell::Ptr>> _cells;
	Render::Texture* _ground;
	bool _showGhosts;
};


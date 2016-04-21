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

class FieldCell {
public:
	typedef boost::intrusive_ptr<FieldCell> Ptr;
	FieldCell();
	~FieldCell();
	
	//FieldCell(const FieldCell&);
	//FieldCell& operator = (const FieldCell&);
	void Init(CellType cell, IPoint size, FPoint position);
	void Draw(Render::Texture* _ground, FRect cTexRect);
	void Draw();
	void Update(float dt);
	CellType Type();
	IPoint Size();
	FPoint Position();
	bool Selected();
	

	


	void SetPos(FPoint);
	void SetSize(IPoint);
	//void SetTexture(std::string);
	void SetType(CellType cell);
	void Select();
	void UnSelect();
	bool AddTower();
	bool DestroyTower();
	bool Empty();
	friend void intrusive_ptr_add_ref(FieldCell*);
	friend void intrusive_ptr_release(FieldCell*);
	void AddRef() {
		++ref_cnt_;
	}
	void Release() {
		if (--ref_cnt_ == 0) {
			delete this;

		}
	}
	void ShowGhosts(bool);
private:
	int ref_cnt_;
	bool _selected;
	bool _empty;
	CellType _cellType;
	IPoint _size;
	FPoint _position;
	//IPoint _coords;
	
	Render::TexturePtr _tex;
	Render::TexturePtr _ghost;
	bool _showGhost;

};

inline void intrusive_ptr_add_ref(FieldCell* e) { e->AddRef(); }
inline void intrusive_ptr_release(FieldCell* e) { e->Release(); }




class FieldMap
{
public:
	FieldMap() ;
	~FieldMap();
	//FieldMap(const FieldMap&);
	//FieldMap& operator = (const FieldMap&);
	void Init();
	void TryInit();
	void LoadFromFile(std::string);
	void LoadFromXml(std::string);
	void SaveToFile(std::string);
	void Draw();
	void Update(float dt);
	bool AddTower(IPoint);
	bool DestroyTower(IPoint);
	//friend void intrusive_ptr_add_ref(FieldMap*);
	//friend void intrusive_ptr_release(FieldMap*);
	//void AddRef() {
	//	++ref_cnt_;
	//}
	//void Release() {
	//	if (--ref_cnt_ == 0) {
	//		delete this;
	//		
	//	}
	//}
	IPoint Size();
	IPoint CellSize();
	CellType PosCellType(IPoint);
	IPoint PosCell(FPoint pos);
	CellType SelectCell(FPoint pos);
	void ShowGhosts(TowerType);
	void Reset();
	std::vector<std::vector<FieldCell::Ptr>> Cells();
private:
	//int ref_cnt_;
	IPoint _size;
	IPoint _cellSize;
	std::vector<std::vector<FieldCell::Ptr>> _cells;
	Render::Texture* _ground;
	bool _showGhosts;
};

//inline void intrusive_ptr_add_ref(FieldMap* e) { e->AddRef(); }
//inline void intrusive_ptr_release(FieldMap* e) { e->Release(); }
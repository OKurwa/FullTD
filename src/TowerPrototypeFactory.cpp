#include "stdafx.h"
#include "TowerParent.h"
#include "TowerPrototypeFactory.h"
using namespace std;
using namespace rapidxml;

//----------------------------------------------//
//----------------------------------------------//
//					Фабрика						//
//----------------------------------------------//
//----------------------------------------------//

TowerPrototypeFactory::TowerPrototypeFactory() {
	_Nloaded = false;
	_Slloaded = false;
	_Sploaded = false;
	_Bloaded = false;
	_Dloaded = false;

}

void TowerPrototypeFactory::Init(std::string xmlfilename) {

	try {
		file<> file(xmlfilename.c_str());
		// Может бросить исключение, если нет файла.
		xml_document<> doc;
		doc.parse<0>(file.data());
		// Может бросить исключение, если xml испорчен.

		xml_node<>* game = doc.first_node();
		if (!game) { Assert(false); throw runtime_error("No root node"); }

		xml_node<>* towers = game->first_node("Towers");
		for (xml_node<>* tower = towers->first_node("Tower"); tower; tower = tower->next_sibling("Tower")) {
			string id = tower->first_attribute("id")->value();

			if (id == "NormalTower") {
				_nPrototype.LoadTowerFormXML(tower);
				//_nPrototype.LoadMissilesFormXML(tower);
			}
			else if (id == "SplashTower") {
				_spPrototype.LoadTowerFormXML(tower);
				//_spPrototype.LoadMissilesFormXML(tower);
			}
			else if (id == "SlowTower") {
				_slPrototype.LoadTowerFormXML(tower);
				//_slPrototype.LoadMissilesFormXML(tower);
			}
			else if (id == "DecayTower") {
				_dPrototype.LoadTowerFormXML(tower);
				//_dPrototype.LoadMissilesFormXML(tower);
			}
			else if (id == "BashTower") {
				_bPrototype.LoadTowerFormXML(tower);
				//_bPrototype.LoadMissilesFormXML(tower);
			}

		}


	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
};

TowerParent::Ptr TowerPrototypeFactory::createTower(TowerType tType)
{
	switch (tType)
	{
	case NORMAL:
		return _nPrototype.Clone();
		break;
	case SPLASH:
		return _spPrototype.Clone();
		break;
	case SLOW:
		return _slPrototype.Clone();
		break;
	case DECAY:
		return _dPrototype.Clone();
		break;
	case BASH:
		return _bPrototype.Clone();
		break;
	default:
		return nullptr;
		break;
	}

}


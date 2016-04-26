#include "stdafx.h"
#include "TowerShop.h"
#include "Menu.h"
using namespace std;
using namespace Xml;
using namespace rapidxml;



// ������� �����
// ������ ���������� � ����� �����, ����� � ������ ������������ ����� ������ ����� � ������ ���������� 
void TowerShop::LoadFromXml(std::string & filename)
{
	try {
		file<> file(filename.c_str());
		// ����� ������� ����������, ���� ��� �����.

		xml_document<> doc;
		doc.parse<0>(file.data());
		// ����� ������� ����������, ���� xml ��������.

		xml_node<>* game = doc.first_node();
		if (!game) { Assert(false); throw runtime_error("No root node"); }

		xml_node<>* towers = game->first_node("Towers");
		xml_node<>* shop = game->first_node("Shop");
		for (xml_node<>* tower = towers->first_node("Tower"); tower; tower = tower->next_sibling("Tower")) {
			LotStruct newLot;
			bool found = false;
			newLot.towerId = GetStringAttributeOrDef(tower, "id", "");
			newLot.towerCost = GetIntAttributeOrDef(tower, "price", 0);

			
			for (xml_node<>* lot = shop->first_node("Lot"); lot; lot = lot->next_sibling("Lot")) {
				std::string tId = GetStringAttributeOrDef(lot, "towerId", "");
				if (newLot.towerId == tId) {
					newLot.buttonId = GetIntAttributeOrDef(lot, "menuButtonId", Button::NO_VALUE);
					found = true;
				}
				if (found)
					break;
			};
			_lots.push_back(newLot);
		};
		LotStruct newLot;
		newLot.towerId = "Destroy";
		for (xml_node<>* lot = shop->first_node("Lot"); lot; lot = lot->next_sibling("Lot")) {
			std::string tId = GetStringAttributeOrDef(lot, "towerId", "");
			if (newLot.towerId == tId) {
				newLot.buttonId = GetIntAttributeOrDef(lot, "menuButtonId", Button::NO_VALUE);
				break;
			}
		};
		_lots.push_back(newLot);
		for (int i = 0; i < _lots.size(); i++) {
			if (_lots[i].towerId == "NormalTower") {
				_lots[i].type = NORMAL;
			}
			else if (_lots[i].towerId == "SplashTower") {
				_lots[i].type = SPLASH;
			}
			else if(_lots[i].towerId == "SlowTower") {
				_lots[i].type = SLOW;
			}
			else if(_lots[i].towerId == "DecayTower") {
				_lots[i].type = DECAY;
			}
			else if(_lots[i].towerId == "BashTower") {
				_lots[i].type = BASH;
			}
			else if(_lots[i].towerId == "Destroy") {
				_lots[i].type = DESTROY;
			}
			
		}
		
	}
	catch (std::exception const& e) {
		Log::log.WriteError(e.what());
		Assert(false);
	}
}

std::vector<int> TowerShop::GetDisabledButtons(int gold)
{
	std::vector<int> toDisable;
	for (int i = 0; i < _lots.size(); i++) {
		if (_lots[i].towerCost > gold) {
			toDisable.push_back(_lots[i].buttonId);
		}
	}
	return  toDisable;
}

TowerType TowerShop::GetTypeFromButton(int id)
{
	if (id == Button::NO_VALUE)
		return EMPTY;
	TowerType res = EMPTY;
	for (int i = 0; i < _lots.size(); i++) {
		if (_lots[i].buttonId == id) {
			res = _lots[i].type;
			break;
		}
	}
	return res;
}

int TowerShop::GetButtonFromType(TowerType type)
{
	int res = Button::NO_VALUE;
	for (int i = 0; i < _lots.size(); i++) {
		if (_lots[i].type == type) {
			res = _lots[i].buttonId;
			break;
		}
	}
	return res;
}

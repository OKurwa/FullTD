#pragma once

class TowerShop
{
public:
	TowerShop() {};
	~TowerShop() {};
	void LoadFromXml(std::string &);
	std::vector<int> GetDisabledButtons(int);
	TowerType GetTypeFromButton(int);
	int GetButtonFromType(TowerType);
private:
	struct LotStruct {
		TowerType type;
		std::string towerId;
		int buttonId;
		int towerCost;
	};
	std::vector<LotStruct> _lots;
};


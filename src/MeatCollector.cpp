#include "stdafx.h"
#include "MeatCollector.h"

MeatCollector::MeatCollector()
{
	_meat = Core::resourceManager.Get<Render::Texture>("Meat");
	_position = IPoint(0,0);
	_flyTimer = 0;
}

MeatCollector::~MeatCollector()
{
}

void MeatCollector::CalulateWay(FPoint a, FPoint b)
{
	FPoint delta = (b - a)/6;
	//FPoint center = a + delta / 2;
	float radius = a.GetDistanceTo(b) / 2;
	//float step = radius/6;
	std::vector<FPoint> arr;
	for (int i = 0; i < 5; i++) {
		FPoint point = FPoint(a.x + delta.x*i, 0);
		point.y = math::sqrt(radius*radius - point.x*point.x);
		arr.push_back(point);
		_spline.addKey(point);
	}

	_spline.CalculateGradient();

}

void MeatCollector::Draw()
{
	_meat->Draw(IRect(_position,20,20),FRect(0,1,0,1));
}

void MeatCollector::Update(float dt)
{
	_flyTimer += dt;
	_position = _spline.getGlobalFrame(_flyTimer).Rounded();
}

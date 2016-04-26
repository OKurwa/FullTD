#include "stdafx.h"
#include "MeatCollector.h"

MeatCollector::MeatCollector()
{
	_meat = Core::resourceManager.Get<Render::Texture>("Meat");
	_startSize = IPoint(10,10);
	_finishSize = IPoint(20, 20);
}

MeatCollector::~MeatCollector()
{
}

void MeatCollector::AddMeat(FPoint a)
{
	Meat tmpMeat;
	tmpMeat._position = a.Rounded();
	tmpMeat._flyTimer = 0;
	tmpMeat._tailEff = nullptr;
	CalulateTimedWay(a, tmpMeat);

}

void MeatCollector::CalulateTimedWay(FPoint a, Meat & meat)
{
	int pointCount = 5;
	float radius = a.GetDistanceTo(_dest) / 2;
	float dx = (_dest.x - a.x) / pointCount;
	float dy = (_dest.y - a.y) / pointCount;
	float zX = -radius;
	meat._pathSplineX.addKey(0, a.x);
	meat._pathSplineY.addKey(0, a.y);
	float dt = 0;
	std::vector<FPoint> arr;
	for (int i = 1; i < pointCount; i++) {
		FPoint point = FPoint(a.x + dx*i, a.y + dy*i);
		zX += dx;
		dt += 1/(float)pointCount;
		point.y += math::sqrt(math::abs(radius*radius - zX*zX));

		if (point.y > Render::device.Height())
			point.y = Render::device.Height() - 3 * meat._size.y;

		meat._pathSplineX.addKey(dt, point.x);
		meat._pathSplineY.addKey(dt, point.y);

	}
	meat._pathSplineX.addKey(1, _dest.x);
	meat._pathSplineY.addKey(1, _dest.y);
	meat._pathSplineX.CalculateGradient();
	meat._pathSplineY.CalculateGradient();
	meat._sizeSpline.addKey(_startSize);
	meat._sizeSpline.addKey(_finishSize);
	meat._sizeSpline.CalculateGradient();
	meat._tailEff = meat._tailCont.AddEffect("MeatEffFly");
	meat._tailEff->Reset();
	meat._tailEff->SetPos(a.Rounded() + (meat._size / 2).Rounded());
	_meats.push_back(meat);
}

void MeatCollector::Draw()
{
	for (int i = 0; i < _meats.size(); i++) {
		Render::BeginAlphaMul(1.0);
		//if (_meats[i]._flyTimer<=1) {
		_meats[i]._tailCont.Draw();
		if(_meats[i]._flyTimer!=1)
			_meat->Draw(IRect(_meats[i]._position, _meats[i]._size.x, _meats[i]._size.y), FRect(0, 1, 0, 1));
		//}
		Render::EndAlphaMul();
	}
	
}

void MeatCollector::Update(float dt)
{
	for (int i = 0; i < _meats.size(); i++) {
		//if (_meats[i]._flyTimer <=1) {
		
		_meats[i]._flyTimer += dt;
		if (_meats[i]._flyTimer > 1)
			_meats[i]._flyTimer = 1;
		_meats[i]._position.x = _meats[i]._pathSplineX.getGlobalFrame(math::EaseInQuad(_meats[i]._flyTimer));
		_meats[i]._position.y = _meats[i]._pathSplineY.getGlobalFrame(math::EaseInQuad(_meats[i]._flyTimer));
		_meats[i]._size = _meats[i]._sizeSpline.getGlobalFrame(_meats[i]._flyTimer).Rounded();
		_meats[i]._tailEff->SetPos(_meats[i]._position + (_meats[i]._size / 2).Rounded());
		
		if (_meats[i]._flyTimer == 1) {
			_meats[i]._tailEff->Finish();
		}
		_meats[i]._tailCont.Update(dt);
			//}

	}
	for (std::vector<Meat>::iterator it = _meats.begin(); it != _meats.end();) {
		if ((*it)._tailEff->isEnd()) {
			it = _meats.erase(it);
		}
		else {
			it++;
		}

	}
	
	
}

void MeatCollector::SetTexSizes(IPoint startSize, IPoint finishSize)
{
	_startSize = startSize;
	_finishSize = finishSize;
}

void MeatCollector::SetDestinationPoint(FPoint b)
{
	_dest = b;
}

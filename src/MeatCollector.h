#pragma once
//Класс эффекта перелета
class MeatCollector
{
public:
	struct Meat {
		IPoint _position;
		IPoint _size;
		float _flyTimer;
		ParticleEffectPtr _tailEff;
		EffectsContainer _tailCont;
		TimedSplinePath _pathSplineX;
		TimedSplinePath _pathSplineY;
		SplinePath<FPoint> _sizeSpline;
		
	};
	MeatCollector();
	~MeatCollector();
	void AddMeat(FPoint a);
	
	void Draw();
	void Update(float dt);
	void SetTexSizes(IPoint startSize, IPoint finishSize);
	void SetDestinationPoint(FPoint b);
private:
	void CalulateWay(FPoint a, Meat & meat);
	void CalulateTimedWay(FPoint a, Meat & meat);
	IPoint _startSize;
	IPoint _finishSize;
	FPoint _dest;
	Render::TexturePtr _meat;
	std::vector<Meat> _meats;
};


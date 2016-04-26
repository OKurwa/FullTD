#pragma once
//Класс эффекта перелета
class MeatCollector
{
public:
	MeatCollector();
	~MeatCollector();
	void CalulateWay(FPoint a, FPoint b);
	void Draw();
	void Update(float dt);
private:
	IPoint _position;
	float _flyTimer;
	Render::TexturePtr _meat;
	ParticleEffectPtr _tailEff;
	EffectsContainer _tailCont;
	SplinePath<FPoint> _spline;
};


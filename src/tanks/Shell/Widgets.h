#pragma once
#include "Text.h"
#include <list>
#include <string>
#include <queue>

class FpsCounter : public UI::Text
{
	std::list<float> _dts;
	std::list<float> _dts_net;
	int _nSprites;
	int _nLights;
	int _nBatches;

public:
	FpsCounter(UI::UIWindow *parent, float x, float y, AlignTextKind align);

protected:
	void OnVisibleChange(bool visible, bool inherited);
	void OnTimeStep(float dt);
};

class Oscilloscope : public UI::UIWindow
{
public:
	Oscilloscope(UI::UIWindow *parent, float x, float y);
	void Push(float value);
	void SetRange(float rmin, float rmax);
	void SetTitle(const std::string &title);
	void SetGridStep(float stepX, float stepY);

	void AutoGrid();
	void AutoRange();

protected:
	void Draw(DrawingContext &dc) const override;

private:
	size_t _barTexture;
	size_t _titleFont;
	std::string _title;
	std::deque<float> _data;
	float _rangeMin;
	float _rangeMax;
	float _gridStepX;
	float _gridStepY;
	float _scale;
};


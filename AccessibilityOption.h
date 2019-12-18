#pragma once
#include "COption.h"
#include "GeoPoint.h"
#include "GeoLayer.h"
class AccessibilityOption
{
public:
	AccessibilityOption();
	~AccessibilityOption();
	void setStartPoint(GeoPoint* point);
	GeoPoint* getStartPoint();
	void setTargetPointLayer(GeoLayer* layer);
	GeoLayer* getTargetPointLayer();
	void setArcLayer(GeoLayer* layer);
	GeoLayer* getArcLayer();
	void setNoteLayer(GeoLayer* layer);
	GeoLayer* getNoteLayer();
	void setTimeLimit(float time);
	float getTimeLimit();
private:
	GeoPoint* startPoint;
	GeoLayer* targetPointLayer;
	GeoLayer* arcLayer;
	GeoLayer* noteLayer;
	float timeLimit;
};


#include "AccessibilityOption.h"



AccessibilityOption::AccessibilityOption()
{
}


AccessibilityOption::~AccessibilityOption()
{
}

void AccessibilityOption::setStartPoint(GeoPoint * point)
{
	this->startPoint = point;
}

GeoPoint * AccessibilityOption::getStartPoint()
{
	return startPoint;
}

void AccessibilityOption::setTargetPointLayer(GeoLayer * layer)
{
	targetPointLayer = layer;
}

GeoLayer * AccessibilityOption::getTargetPointLayer()
{
	return targetPointLayer;
}

void AccessibilityOption::setArcLayer(GeoLayer * layer)
{
	arcLayer = layer;
}

GeoLayer * AccessibilityOption::getArcLayer()
{
	return arcLayer;
}

void AccessibilityOption::setNoteLayer(GeoLayer * layer)
{
	noteLayer = layer;
}

GeoLayer * AccessibilityOption::getNoteLayer()
{
	return arcLayer;
}

void AccessibilityOption::setTimeLimit(float time)
{
	timeLimit = time;
}

float AccessibilityOption::getTimeLimit()
{
	return timeLimit;
}

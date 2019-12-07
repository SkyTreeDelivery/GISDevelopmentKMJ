#include "Render.h"

Render::Render()
{

}

Render::~Render()
{
}

void Render::draw(QList<GeoGeometry *> objs)
{

}

void Render::setMarkerSymbol(MarkerSymbol * markerSymbol)
{
	this->markerSymbol = markerSymbol;
}

void Render::setLineSymbol(LineSymbol * lineSymbol)
{
	this->lineSymbol = lineSymbol;
}

void Render::setFillSymbol(FillSymbol * fillSymbol)
{
	this->fillSymbol = fillSymbol;
}

MarkerSymbol * Render::getMarkerSymbol()
{
	return markerSymbol;
}

LineSymbol * Render::getLineSymbol()
{
	return lineSymbol;
}

FillSymbol * Render::getFillSymbol()
{
	return fillSymbol;
}

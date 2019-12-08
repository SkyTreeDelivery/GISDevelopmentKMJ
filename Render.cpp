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

Symbol* Render::setMarkerSymbol(MarkerSymbol * markerSymbol)
{
	Symbol* old = this->markerSymbol;
	this->markerSymbol = markerSymbol;
	return old;
}

Symbol* Render::setLineSymbol(LineSymbol * lineSymbol)
{
	Symbol* old = this->lineSymbol;
	this->lineSymbol = lineSymbol;
	return old;
}

Symbol* Render::setFillSymbol(FillSymbol * fillSymbol)
{
	Symbol* old = this->fillSymbol;
	this->fillSymbol = fillSymbol;
	return old;
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

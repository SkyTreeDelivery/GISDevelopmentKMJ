#include "MarkerSymbol.h"



MarkerSymbol::MarkerSymbol()
{
}


MarkerSymbol::~MarkerSymbol()
{
}

MarkerSymbol::MarkerSymbol(int color, float size)
{
	this->inColor = color;
	this->size = size;
}

MarkerSymbol::MarkerSymbol(int color, float size, LineSymbol * outline)
{
	this->inColor = color;
	this->size = size;
	this->outline = outline;
}

void MarkerSymbol::setOutline(LineSymbol * outline)
{
	
	this->outline = outline;
	
}

LineSymbol * MarkerSymbol::getOutline()
{
	return this->outline;
}

void MarkerSymbol::setColor(int color)
{
	this->inColor = color;
}

int MarkerSymbol::getColor()
{
	return this->inColor;
}

void MarkerSymbol::setSize(float size)
{
	this->size = size;
}

float MarkerSymbol::getSize()
{
	return this->size;
}

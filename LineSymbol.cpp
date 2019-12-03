#include "LineSymbol.h"



LineSymbol::LineSymbol()
{
}


LineSymbol::~LineSymbol()
{
}

LineSymbol::LineSymbol(int color, float width)
{
	this->color = color;
	this->width = width;
}

int LineSymbol::getColor()
{
	return this->color;
}

void LineSymbol::setColor(int color)
{
	this->color = color;
}

float LineSymbol::getWidth()
{
	return this->width;
}

void LineSymbol::setWidth(float width)
{
	this->width = width;
}

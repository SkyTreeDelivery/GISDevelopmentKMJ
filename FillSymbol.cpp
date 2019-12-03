#include "FillSymbol.h"



FillSymbol::FillSymbol()
{
}


FillSymbol::~FillSymbol()
{
}

FillSymbol::FillSymbol(int color, LineSymbol * outline)
{
	this->inColor = color;
	this->outline = outline;
}

void FillSymbol::setOutline(LineSymbol * outline)
{
	this->outline = outline;
}

LineSymbol * FillSymbol::getOutline()
{
	return this->outline;
}

void FillSymbol::setColor(int color)
{
	this->inColor = color;
}

int FillSymbol::getColor()
{
	return this->inColor;
}

#pragma once
#include "Symbol.h"
#include "LineSymbol.h"
class MarkerSymbol :
	public Symbol
{
public:
	MarkerSymbol();
	~MarkerSymbol();
	MarkerSymbol(int color, float size);
	MarkerSymbol(int color, float size, LineSymbol* outline);
	void setOutline(LineSymbol* outline);
	LineSymbol* getOutline();
	void setColor(int color);
	int getColor();
	void setSize(float size);
	float getSize();
private:
	LineSymbol* outline;
	int inColor;
	float size;
};


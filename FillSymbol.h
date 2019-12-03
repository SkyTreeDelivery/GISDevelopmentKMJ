#pragma once
#include "Symbol.h"
#include "LineSymbol.h"
class FillSymbol :
	public Symbol
{
public:
	FillSymbol();
	~FillSymbol();
	FillSymbol(int color, LineSymbol* outline);
	void setOutline(LineSymbol* outline);
	LineSymbol* getOutline();
	void setColor(int color);
	int getColor();
private:
	LineSymbol* outline;
	int inColor;
};


#pragma once
class LineSymbol
{
public:
	LineSymbol();
	~LineSymbol();
	LineSymbol(int color, float width);
	int getColor();
	void setColor(int color);
	float getWidth();
	void setWidth(float width);
private:
	int color;
	float width;
};


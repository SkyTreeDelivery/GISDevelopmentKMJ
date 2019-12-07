#pragma once
class EnumType
{
public:
	EnumType(void);
	~EnumType(void);
	enum geometryType {POINT, POLYLINE, POLYGON};
	enum resizeDirection {LEFT,TOP,RIGHT,BOTTOM,LEFTTOP,RIGHTTOP,RIGHTBOTTOM,LRFTBOTTOM,NOCHANGE};
	enum symbolType {MARKERSYMBOL,LINESYMBOL,FILLSYMBOL};
	enum bufferType {VBO,EBO};
};


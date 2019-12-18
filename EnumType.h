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
	enum styleType {FILLCOLOR,LINEWIDTH,LINECOLOR,MARKERWIDTH,MARKETCOLOR};
	enum source {GEOJSON,SHAPEFILE,POSTGRESQL};
	enum indexMode{GRIDINDEX,QUADTREE,R};//�����������Ĳ���������R+����
	enum quadTree{TOPRIGHT,BOTTOMRIGHT,BOTTOMLEFT,TOPLEFT};//�Ĳ������ĸ�����
};


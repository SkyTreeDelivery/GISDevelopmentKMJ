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
	enum indexMode{GRIDINDEX,QUADTREE,R};//网格索引、四叉树索引、R+索引
	enum quadTree{TOPRIGHT,BOTTOMRIGHT,BOTTOMLEFT,TOPLEFT};//四叉树的四个象限
};


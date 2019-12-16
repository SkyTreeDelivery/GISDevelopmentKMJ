#pragma once
class EnumType
{
public:
	EnumType(void);
	~EnumType(void);
	enum geometryType {POINT, POLYLINE, POLYGON};
	enum resizeDirection {LEFT,TOP,RIGHT,BOTTOM,LEFTTOP,RIGHTTOP,RIGHTBOTTOM,LRFTBOTTOM,NOCHANGESIZE};
	enum symbolType {MARKERSYMBOL,LINESYMBOL,FILLSYMBOL};
	enum bufferType {VBO,EBO};
	enum renderType {MARKER_FILL,MARKER_LINE,LINE_LINE,FILL_FILL,FILL_LINE};
	enum styleType {FILLCOLOR,LINEWIDTH,LINECOLOR,MARKERWIDTH,MARKETCOLOR};
	enum source {GEOJSON,SHAPEFILE,POSTGRESQL};
	enum operateMode {NORMOL,IDENTIFY};
	enum selectMode {SINGLEMODE,MULTIMODE};
	enum dataChangedType {NOCHANGEDATA,SPATICALDATA,COLORDATA};
};


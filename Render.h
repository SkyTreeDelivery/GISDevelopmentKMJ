#ifndef RENDER_H
#define RENDER_H
#include <QList>
#include "GeoGeometry.h"
#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "GeoPolyline.h"
#include "Symbol.h"
#include "MarkerSymbol.h"
#include "LineSymbol.h"
#include "FillSymbol.h"

class Render
{
public:
    Render();
	~Render();
    void draw(QList<GeoGeometry*> objs);
	Symbol* setMarkerSymbol(MarkerSymbol* markerSymbol);
	Symbol* setLineSymbol(LineSymbol* lineSymbol);
	Symbol* setFillSymbol(FillSymbol* fillSymbol);
	MarkerSymbol* getMarkerSymbol();
	LineSymbol* getLineSymbol();
	FillSymbol* getFillSymbol();
private:
	MarkerSymbol* markerSymbol;
	LineSymbol* lineSymbol;
	FillSymbol* fillSymbol;
};

#endif // RENDER_H

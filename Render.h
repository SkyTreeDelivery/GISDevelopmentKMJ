#ifndef RENDER_H
#define RENDER_H
#include <QList>
#include "GeoGeometry.h"
#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "GeoPolyline.h"


class Render
{
public:
    Render();
	~Render();
    void draw(QList<GeoGeometry*> objs);
};

#endif // RENDER_H

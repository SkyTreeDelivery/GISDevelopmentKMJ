#include "geopolyline.h"
#include "EnumType.h"

GeoPolyline::GeoPolyline()
{
    this->type = EnumType::POLYLINE;
}

GeoPolyline::~GeoPolyline()
{
	for(int i = 0; i < size(); i++){
		delete pts.at(i);
	}
}

void GeoPolyline::addPoint(GeoPoint * point)
{
	pts.push_back(point);
}

GeoPoint * GeoPolyline::getPointAt(int idx)
{
	return pts.at(idx);
}

GeoPoint * GeoPolyline::removeAt(int idx)
{
	GeoPoint* p = pts.at(idx);
	pts.removeAt(idx);
	return p;
}

QList<GeoPoint*> GeoPolyline::removeAll()
{
	pts.clear();
	return pts;
}

int GeoPolyline::size()
{
	return pts.size();
}

QRectF GeoPolyline::getRect()
{
	GeoPoint* firstPoint = getPointAt(0);
	float left = firstPoint->getXf();
	float top = firstPoint->getYf();
	float right = firstPoint->getXf();
	float bottom = firstPoint->getYf();
	for(int i = 0; i < size(); i++){
		GeoPoint* p = getPointAt(i);
		if(p->getXf() < left) left = p->getXf();
		if(p->getYf() > top) top = p->getYf();
		if(p->getXf() > right) right = p->getXf();
		if(p->getYf() < bottom) bottom = p->getYf();
	}
	return QRectF(QPointF(left,top),QPointF(right,bottom));
}

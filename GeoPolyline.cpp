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

double GeoPolyline::disToPoint(GeoPoint *pt)
{
	double x1, y1, x2, y2;
	double x0 = pt->getXf();
	double y0 = pt->getYf();
	double mindis, dis;
	for (int i = 0; i < size() - 1; i++)
	{
		x1 = pts.at(i)->getXf();
		y1 = pts.at(i)->getYf();
		x2 = pts.at(i + 1)->getXf();
		y2 = pts.at(i + 1)->getYd();

		dis = ((x0 - x1)*(y2 - y1) + (y0 - y1)*(x1 - x2)) / pow(pow(y2 - y1, 2) + pow(x1 - x2, 2), 0.5);

		if (i == 0)
			mindis = dis;
		else
		{
			if (mindis > dis)
				mindis = dis;
		}
	}
	return mindis;
}

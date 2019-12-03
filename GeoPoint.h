#ifndef GEOPOINT_H
#define GEOPOINT_H
#include "GeoGeometry.h"
#include <qrect.h>

class GeoPoint : public GeoGeometry
{
public:
    GeoPoint();
	~GeoPoint();
	double getXf();
	void setXf(double x);
	double getYf();
	void setYf(double y);
	int getXd();
	void setXd(int x);
	int getYd();
	void setYd(int y);
	void setXYf(double x, double y);
	QRectF getRect();
	int size();
private:
    double x;
    double y;
};

#endif // GEOPOINT_H

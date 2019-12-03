#include "geopoint.h"
#include "EnumType.h"

GeoPoint::GeoPoint()
{
    this->type = EnumType::POINT;
}

GeoPoint::~GeoPoint()
{
	
}

double GeoPoint::getXf()
{
	return x;
}

void GeoPoint::setXf(double x)
{
	this->x = x;
}

double GeoPoint::getYf()
{
	return y;
}

void GeoPoint::setYf(double y)
{
	this->y = y;
}

int GeoPoint::getXd()
{
	return (int)x;
}

void GeoPoint::setXd(int x)
{
	this->x = (double)x;
}

int GeoPoint::getYd()
{
	return (int)y;
}

void GeoPoint::setYd(int y)
{
	this->y = (double)y;
}

void GeoPoint::setXYf(double x, double y)
{
	this->x = x;
	this->y = y;
}

QRectF GeoPoint::getRect()
{
	return QRectF(x,y,0,0);
}

int GeoPoint::size()
{
	return 1;
}


#include "GeoFeature.h"


GeoFeature::GeoFeature(void):geometry(NULL)
{
}


GeoFeature::~GeoFeature(void)
{
	delete geometry;
	delete attriMap;
}

GeoGeometry * GeoFeature::getGeometry()
{
	return geometry;
}

void GeoFeature::setGemetry(GeoGeometry * geometry)
{
	this->geometry = geometry;
}

QMap<QString, QVariant>* GeoFeature::getAttributeMap()
{
	return attriMap;
}

void GeoFeature::setAttributeMap(QMap<QString, QVariant>* map)
{
	this->attriMap = map;
}

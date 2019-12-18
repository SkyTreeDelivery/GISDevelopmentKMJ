#ifndef GEOMAP_H
#define GEOMAP_H
#include <QList>
#include "GeoLayer.h"

class GeoMap
{
public:
    GeoMap();
	~GeoMap();
	GeoLayer* getLayerAt(int i);
	GeoLayer* getLayerByFullpath(QString fullpath);
	void addLayer(GeoLayer* layer);
	GeoLayer* removeLayerAt(int idx);
	GeoLayer* remove(GeoLayer* layer);
	QList<GeoLayer*> removeAll();
	int getIdx(GeoLayer* layer);
	void removeByFullpath(QString fullpath);
	int size();
	void moveLayerLevel(int from, int to);

private:
    QList<GeoLayer*> layers;
};

#endif // GEOMAP_H

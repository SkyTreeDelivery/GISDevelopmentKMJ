#ifndef GEOLAYER_H
#define GEOLAYER_H
#include <QList>
#include "Render.h"
#include "EnumType.h"
#include "GeoGeometry.h"
#include "GeoFeature.h"

class GeoLayer
{
public:
    GeoLayer();
	~GeoLayer();
	GeoFeature* getFeatureAt(int i);
	void addFeature(GeoFeature* feature);
	GeoFeature* removeFeatureAt(int idx);
	QList<GeoFeature*> removeAll();
	Render* setRender(Render* render);
	Render* getRender();
	int size();
	void setType(int type);
	int getType();
	QString getName();
	void setName(QString name);
	QString getTypeString();
	QRectF getRect();
	bool isVisable();
	void setVisable(bool visibility);
	QString getFullPath();
	void setFullPath(QString fullpath);
    void draw();
	bool isWaitingRendered();
	void setWaitingRendered(bool b);
	void bindDefaultRender();
private:
    QList<GeoFeature*> features;
    Render* render;
	int type;   //一层只能添加一种数据
	QString name;
	bool visibility;
	QString fullpath;
	bool waitingRendered;
};

#endif // GEOLAYER_H

#ifndef GEOLAYER_H
#define GEOLAYER_H
#include <QList>
#include <QtNetwork/qnetworkrequest.h>
#include<QtNetwork/qnetworkreply.h>
#include<QtNetwork/qnetworkaccessmanager.h>
#include "Render.h"
#include "EnumType.h"
#include "GeoGeometry.h"
#include "GeoFeature.h"
#include"GridIndex.h"
#include"gpc/gpc.h"

class GeoLayer
{
public:
    GeoLayer();
	~GeoLayer();
	GeoFeature* getFeatureAt(int i);
	QList<GeoFeature*> getAllFeature();
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
	void setSource(int source);
	int getSource();
	QString getSourceName();

//与空间查询相关的：
	//识别
	GeoFeature *Identify(GeoPoint *point, GeoLayer *layer, int threshold);
	//查询
	QList<GeoFeature*> search(GeoLayer* layer, QString attriName, QString attriValue);
	QList<QString> getAttriNames(GeoLayer *layer);
    //获取和添加索引类型
	void setIndexMode(int mode);
	int getIndexMode();
	//添加索引
	void setSpatialIndex(Index *idx);

private:
    QList<GeoFeature*> features;
    Render* render;
	int type;   //一层只能添加一种数据
	QString name;
	bool visibility;
	QString fullpath;
	bool waitingRendered;
	int source;

	//索引
	int indexMode;//索引模式
	Index *spatialIndex;
};

#endif // GEOLAYER_H

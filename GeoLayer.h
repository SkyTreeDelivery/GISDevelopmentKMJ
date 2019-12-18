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
#include "QuadTree.h"
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
	void setSource(int source);
	int getSource();
	QString getSourceName();
	QList<QString> getAttributeNames();
	void setSelectMode(int mode);
	int getSelectMode();
	void setDataChangedType(int type);
	int getDataChangedType();
	void bindDefaultRender();
	void setAttributeNames(QList<QString> names);
	
//与空间查询相关的：
	//识别
	GeoFeature *Identify(GeoPoint *point, GeoLayer *layer, int threshold);
	//查询
	QList<GeoFeature*> search(GeoLayer* layer, QString attriName, QString attriValue);
	QList<QString> getAttriNames();
    //获取和添加索引类型
	void setIndexMode(int mode);
	int getIndexMode();
	//添加索引
	void setSpatialIndex(Index *idx);
	
	void setSelectionColor(QColor color);  //所有的要素使用同种色彩与线宽进行配置
	void setSelectionWidth(float width);
	void setSelectionconfiguration(QColor color,float width);
	void selectFeature(GeoFeature* feature);
	QList<GeoFeature*> getSelectedFeatures();
	bool hasSelected(GeoFeature* feature);
	void clearFeatures();
private:
	int type;   //一层只能添加一种数据
    QList<GeoFeature*> features;
    Render* render;
	QString name;
	QString fullpath;
	bool visibility;
	int source;  //显示数据的完整url
	QList<QString> layerAttributeNames;  //属性名列表，暂时替代table的功能
	QList<GeoFeature*> selectedFeatures;
	int selectMode;
	int dataChangeType;
	//索引
	int indexMode;//索引模式
	Index *spatialIndex;
};

#endif // GEOLAYER_H

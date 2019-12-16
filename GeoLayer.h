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

	int size();
	void bindDefaultRender();
	void setAttributeNames(QList<QString> names);
	//识别
	GeoFeature* identify(GeoPoint* point, GeoLayer* layer);
	//查询
	QList<GeoFeature*> search(GeoLayer* layer,QString attriName, QString attriValue);
	QList<QString> getAttriNames(GeoLayer* layer);
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
};

#endif // GEOLAYER_H

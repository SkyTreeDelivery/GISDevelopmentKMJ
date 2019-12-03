#include "util.h"
#include <QFileDialog>
#include <QWidget>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "GeoPoint.h"
#include "geopolygon.h"
#include "GeoPolyline.h"
#include "GeoLayer.h"
#include "EnumType.h"

util::util()
{

}

QString util::openFileDialog(QWidget* widget, QString curPath)
{
    QString dlgTitle = "open a file";
    return QFileDialog::getOpenFileName(widget,dlgTitle,curPath);
}


//TODO 点线面测试完成
GeoLayer* util::openGeoJson(QString path)
{

	QFile file(path);
    if (!file.exists()) //文件不存在
        return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
	QByteArray text  = file.readAll();
	QJsonParseError error;

	//注意，大坑，不能有中文，同时fromJson作为一个file指针， 在不移动指针位置的情况下只能使用一次
	//果然有问题还是得上geogle，一般问题还好，稍微偏门一点的问题就不行了
    QJsonDocument jsonDocu = QJsonDocument::fromJson(text,&error);
    QJsonObject jsonData = jsonDocu.object();
    QJsonArray featuresJ = jsonData["features"].toArray();
	GeoLayer* layer = new GeoLayer();
	layer->setFullPath(file.fileName());
    for(int i = 0; i < featuresJ.size(); i++){
		QJsonObject featureJ = featuresJ.at(i).toObject();
		GeoFeature* feature = new GeoFeature();
		feature->setAttributeMap(new QMap<QString, QVariant>());
		//TODO 读取属性

		//读取geometry
		QJsonObject geometryJ = featureJ["geometry"].toObject();
		QString type = geometryJ.value("type").toString();
		if(type == "Polygon"){
			layer->setType(EnumType::POLYGON);
			QJsonArray polygonWithHoleJ = geometryJ["coordinates"].toArray(); // [[[x1,y1],[x2,y2]]]
			GeoPolygon* polygon = new GeoPolygon();
			//实际上我现在的只有一个polygon，也就是没有洞和环
			for(int j = 0; j < polygonWithHoleJ.size(); j++){
			   QJsonArray polygonJ = polygonWithHoleJ.at(j).toArray(); // [[x1,y1],[x2,y2]]
			   for(int z = 0; z < polygonJ.size();z++){  // [x1,y1]
				   QJsonArray pointJ = polygonJ.at(z).toArray();
				   double x = pointJ.at(0).toDouble();
				   double y = pointJ.at(1).toDouble();
				   GeoPoint* point = new GeoPoint();
				   point->setXYf(x,y);
				   polygon->addPoint(point);
			   }
			}
			feature->setGemetry(polygon);
			layer->addFeature(feature);
		}else if(type == "LineString"){
			layer->setType(EnumType::POLYLINE);
			QJsonArray polylineJ = geometryJ["coordinates"].toArray(); // [[x1,y1],[x2,y2]]
			GeoPolyline* polyline = new GeoPolyline();
			for(int z = 0; z < polylineJ.size();z++){  // [x1,y1]
				QJsonArray pointJ = polylineJ.at(z).toArray();
				double x = pointJ.at(0).toDouble();
				double y = pointJ.at(1).toDouble();
				GeoPoint* point = new GeoPoint();
				point->setXYf(x,y);
				polyline->addPoint(point);
			}
			feature->setGemetry(polyline);
			layer->addFeature(feature);
		}else if(type == "Point"){
			layer->setType(EnumType::POINT);
			QJsonArray pointJ = geometryJ["coordinates"].toArray(); // [x1,y1]
			double x = pointJ.at(0).toDouble();
			double y = pointJ.at(1).toDouble();
			GeoPoint* point = new GeoPoint();
			point->setXYf(x,y);
			feature->setGemetry(point);
			layer->addFeature(feature);
		}

    }
	if(layer->size()){
		return layer;
	}
    return NULL;
}



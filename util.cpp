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
#include <string>
#include "cjson/CJsonObject.hpp"
#include <vector>
#include <qdebug.h>
#include <QtXml/qdom.h>
#include <sstream>
#include <qrgb.h>
using namespace std;

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
    QJsonDocument jsonDocu = QJsonDocument::fromJson(QString::fromLocal8Bit(text).toLocal8Bit(),&error);
	//QJsonDocument jsonDocu = QJsonDocument::fromJson(text, &error);
    QJsonObject jsonData = jsonDocu.object();
    QJsonArray featuresJ = jsonData["features"].toArray();
	GeoLayer* layer = new GeoLayer();
	layer->bindDefaultRender();
	layer->setFullPath(file.fileName());
    for(int i = 0; i < featuresJ.size(); i++){
		QJsonObject featureJ = featuresJ.at(i).toObject();
		GeoFeature* feature = new GeoFeature();
		QMap<QString, QVariant>* map = feature->getAttributeMap();
		//TODO 读取属性
		QJsonObject attrsJ = featureJ["properties"].toObject();
		/*QStringList keys = attrsJ.keys();
		for (int i = 0; i < keys.length(); i++) {
		    QString key = keys.at(i);
			QString value = attrsJ[key].toString();
			qDebug() << key << ":" << value;
			map->insert(key, QVariant(value));
		}*/ // 这种方法读不出来170，只能读取字符串形式的"170"

		QVariantMap attriMap = attrsJ.toVariantMap();
		QList<QString> keys = attriMap.keys();
		for (int i = 0; i < keys.size(); i++) {
			map->insert(keys.at(i), attriMap[keys.at(i)]);
		}
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

GeoLayer * util::openGeoJsonByCJson(QString path)
{
	QFile file(path);
	if (!file.exists()) //文件不存在
		return false;
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QByteArray qtext = file.readAll();
	string text = qtext.toStdString();
	neb::CJsonObject ojson(text);
	GeoLayer* layer = new GeoLayer();
	layer->setFullPath(file.fileName());
	neb::CJsonObject featuresJ = ojson["features"];
	for (int i = 0; i < featuresJ.GetArraySize(); i++) {
		neb::CJsonObject featureJ = featuresJ[i];
		GeoFeature* feature = new GeoFeature();
		QMap<QString, QVariant>* map = feature->getAttributeMap();
		neb::CJsonObject propertiesJ = featureJ["properties"];
		vector<string> keys = propertiesJ.keys();
		//读取属性数据
		for (int i = 0; i < keys.size(); i++) {
			string key = keys.at(i);
			string value;
			propertiesJ.Get(i, value);
			map->insert(QString::fromStdString(key), QVariant(QString::fromStdString(value)));
		}
		//读取空间数据
		neb::CJsonObject geometryJ = featureJ["geometry"];
		string type;
		geometryJ.Get("type", type);
		if (type == "Polygon") {
			layer->setType(EnumType::POLYGON);
			neb::CJsonObject polygonJWithHole = geometryJ["coordinates"];
			neb::CJsonObject polygonJ = polygonJWithHole[0];
			GeoPolygon* polygon = new GeoPolygon();
			for (int i = 0; i < polygonJ.GetArraySize();i++) {
				double x;
				double y;
				polygonJ[i].Get(0, x);
				polygonJ[i].Get(1, y);
				GeoPoint* point = new GeoPoint();
				point->setXYf(x, y);
				polygon->addPoint(point);
			}
			feature->setGemetry(polygon);
			layer->addFeature(feature);
		}
		else if (type == "LineString") {
			layer->setType(EnumType::POLYLINE);
			neb::CJsonObject polylineJ = geometryJ["coordinates"];
			GeoPolyline* polyline = new GeoPolyline();
			for (int i = 0; i < polylineJ.GetArraySize(); i++) {
				double x;
				double y;
				polylineJ[i].Get(0, x);
				polylineJ[i].Get(1, y);
				GeoPoint* point = new GeoPoint();
				point->setXYf(x, y);
				polyline->addPoint(point);
			}
			feature->setGemetry(polyline);
			layer->addFeature(feature);
		}
		else if (type == "Point") {
			layer->setType(EnumType::POINT);
			neb::CJsonObject pointJ = geometryJ["coordinates"];
			double x;
			double y;
			pointJ.Get(0, x);
			pointJ.Get(1, y);
			GeoPoint* point = new GeoPoint();
			point->setXYf(x, y);
			feature->setGemetry(point);
			layer->addFeature(feature);
		}
	}
	if (layer->size()) {
		return layer;
	}
	return nullptr;
}

FillSymbol* util::parseSLD_Fill(QString path)
{
	//打开或创建文件
	QFile file(path);
	if (!file.open(QFile::ReadOnly)) {
		return NULL;
	}
	QDomDocument document;
	if (!document.setContent(&file)) {
		file.close();
		return NULL;
	}
	FillSymbol* fillSymbol = new FillSymbol();
	LineSymbol* lineSymbol = new LineSymbol();
	fillSymbol->setOutline(lineSymbol);

	QDomElement root = document.documentElement();
	QDomElement namedLayer = root.firstChildElement("NamedLayer");
	QDomElement userStyle = namedLayer.firstChildElement("UserStyle");
	QDomElement featureTypeStyle = userStyle.firstChildElement("FeatureTypeStyle");
	QDomElement rule = featureTypeStyle.firstChildElement("Rule");
	QDomElement polygonSymbolizer = rule.firstChildElement("PolygonSymbolizer");
	QDomElement fill = polygonSymbolizer.firstChildElement("Fill");
	for (QDomElement cssPara = fill.firstChildElement("CssParameter"); !cssPara.isNull(); cssPara = cssPara.nextSiblingElement("CssParameter")) {
		QString attrName = cssPara.attribute("name");
		if (attrName == "fill") {
			QString fillColor = cssPara.text();
			fillSymbol->setColor(color24BitTo256(fillColor));
		}
		else if (attrName == "") {

		}
	}
	QDomElement stroke = polygonSymbolizer.firstChildElement("Stroke");
	for (QDomElement cssPara = stroke.firstChildElement("CssParameter"); !cssPara.isNull(); cssPara = cssPara.nextSiblingElement("CssParameter")) {
		QString attrName = cssPara.attribute("name");
		if (attrName == "stroke") {
			QString outlineColor = cssPara.text();
			lineSymbol->setColor(color24BitTo256(outlineColor));
		}
		else if (attrName == "stroke-width") {
			QString outlineWidth = cssPara.text();
			lineSymbol->setWidth(outlineWidth.toFloat());
		}
	}
	return fillSymbol;
}

QColor util::color24BitTo256(QString color)   // #AAAAAA -> [200,200,200,98]
{
	return parseColor(color);
}

/*vector<unsigned char> util::hexToBin(const string &hex)
{
	std::vector<unsigned char> dest;
	auto len = hex.size();
	dest.reserve(len / 2);
	for (decltype(len) i = 0; i < len; i += 2)
	{
		unsigned int element;
		istringstream strHex(hex.substr(i, 2));
		strHex >> std::hex >> element;
		dest.push_back(static_cast<unsigned char>(element));
	}
	return dest;
}*/

/*QByteArray util::hexToBinInQt(QString hex)
{
	QByteArray binArray;
	std::vector<unsigned char> dest = hexToBin(hex.toStdString());
	for (int i = 0; i < dest.size(); i++) {
		unsigned char cha = dest.at(i);
		binArray.append(cha);
	}
	return binArray;
}*/

QColor util::parseColor(QString hexNamedColor)
{
	QColor color;
	color.setNamedColor(hexNamedColor); //默认透明度为65565,但这只是内部储存的数字，实际范围rgba都是[0-255]
	return color;
}

void util::tesselation(GeoPolygon* opolygon, gpc_tristrip* tristrip)
{
	int vertexNum = opolygon->size();

	//将QVector中的顶点转移到gpc_vertex数组中
	gpc_vertex* vertices = new gpc_vertex[vertexNum];
	for (int i = 0; i < vertexNum; i++)
	{
		GeoPoint* point = opolygon->getPointAt(i);
		vertices[i].x = point->getXf();
		vertices[i].y = point->getYf();
	}

	//生成gpc_vertex_list
	gpc_vertex_list vertexList;
	vertexList.num_vertices = vertexNum;//顶点的个数
	vertexList.vertex = vertices;//顶点序列

	//生成gpc_polygon
	gpc_polygon polygon;
	polygon.num_contours = 1;//有一条轮廓
	polygon.hole = nullptr;//没有孔洞
	polygon.contour = &vertexList;

	//获取分解后的三角形
	tristrip->num_strips = 0;
	tristrip->strip = nullptr;//分解后的三角形顶点是连续接壤的<0,1,2><1,2,3><2,3,4>...

	//三角剖分多边形
	gpc_polygon_to_tristrip(&polygon, tristrip);
}

/*QByteArray util::setDefaultAlpha(QByteArray color)
{
	if (color.size() == 3) {
		for (int i = 0; i < 256; i++) {
			QString hex = QString::number(i, 16);
			color.push_back(hexToBinInQt(hex)); //默认透明度为100
		}
	}
	return color;
}*/





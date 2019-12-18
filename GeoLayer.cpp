#include "geolayer.h"
#include "EnumType.h"
#include "util.h"

GeoLayer::GeoLayer():render(NULL),visibility(true)
{
	type = -1;
	indexMode = EnumType::QUADTREE;//layer默认创建网格索引
}

GeoLayer::~GeoLayer()
{
	for(int i = 0; i < size(); i++){
		delete features.at(i);
	}
	if(render){
		delete render;
	}
}

GeoFeature * GeoLayer::getFeatureAt(int i)
{
	return features.at(i);
}

QList<GeoFeature*> GeoLayer::getAllFeature()
{
	return features;
}

void GeoLayer::addFeature(GeoFeature * feature)
{
	features.push_back(feature);
}

GeoFeature * GeoLayer::removeFeatureAt(int idx)
{
	GeoFeature* obj = features.at(idx);
	features.removeAt(idx);
	return obj;
}

QList<GeoFeature*> GeoLayer::removeAll()
{
	QList<GeoFeature*> copyList = features;
	features.clear();
	return copyList;
}

Render* GeoLayer::setRender(Render * newRender)
{
	Render* oldRender = NULL;
	if (this->render) {
		oldRender = this->render;
	}
	this->render = newRender;
	return oldRender;
}

Render * GeoLayer::getRender()
{
	return this->render;
}

int GeoLayer::size()
{
	return features.size();
}

void GeoLayer::setType(int type)
{
	this->type = type;
}

int GeoLayer::getType()
{
	return type;
}

QString GeoLayer::getName()
{
	return name;
}

void GeoLayer::setName(QString name)
{
	this->name = name;
}

QString GeoLayer::getTypeString()
{
	if (type == EnumType::POINT) {
		return "Point";
	}
	else if (type == EnumType::POLYLINE) {
		return "Polyline";
	}
	else if (type == EnumType::POLYGON) {
		return "Polygon";
	}
	return "Non_Such_Type";
}

//由于QTCreater不是采用定时编译，也就不能实时地告知编写错误，也不能对没有编译的变量进行提示，如果想要自动提示新添加的变量，就需要进行一次编译

void GeoLayer::draw()
{

}

bool GeoLayer::isWaitingRendered()
{
	return this->waitingRendered;
}

void GeoLayer::setWaitingRendered(bool b)
{
	this->waitingRendered = b;
}

void GeoLayer::bindDefaultRender()
{
	Render* render = new Render();
	LineSymbol* lineSymbol = new LineSymbol();
	MarkerSymbol* markerSymbol = new MarkerSymbol();
	FillSymbol* fillSymbol = new FillSymbol();
	QColor black;
	QColor white;
	QColor yellow;
	QColor EEEEEE;
	black.setNamedColor("black");
	white.setNamedColor("white");
	yellow.setNamedColor("yellow");
	EEEEEE.setNamedColor("#EEEEEE");
	lineSymbol->setWidth(1);
	lineSymbol->setColor(black);
	markerSymbol->setColor(yellow);
	markerSymbol->setSize(1);
	markerSymbol->setOutline(lineSymbol);
	fillSymbol->setColor(EEEEEE);
	fillSymbol->setOutline(lineSymbol);

	render->setMarkerSymbol(markerSymbol);
	render->setFillSymbol(fillSymbol);
	render->setLineSymbol(lineSymbol);
	this->render = render;
}

void GeoLayer::setSource(int source)
{
	this->source = source;
}

int GeoLayer::getSource()
{
	return this->source;
}

QString GeoLayer::getSourceName()
{
	if (this->source == EnumType::source::GEOJSON) return "GeoJson";
	else if (this->source == EnumType::source::SHAPEFILE) return "ShapeFile";
	else if (this->source == EnumType::source::POSTGRESQL) return "PostgreSql";
}

GeoFeature * GeoLayer::Identify(GeoPoint * point, GeoLayer * layer,int threshold)
{
	GeoFeature *featureFound;
	if (layer->indexMode == EnumType::GRIDINDEX)
		featureFound = static_cast<GridIndex*>(spatialIndex)->searchGrid(point, threshold);
	else if (layer->indexMode == EnumType::QUADTREE)
		featureFound = static_cast<QuadTree*>(spatialIndex)->SearchQuadTree(point, threshold);
	return featureFound;
}

QList<GeoFeature*> GeoLayer::search(GeoLayer * layer, QString attriName, QString attriValue)
{
	QList<GeoFeature*>features = layer->getAllFeature();
	QList<GeoFeature*>featuresFound;//保存找到的要素
	for (int i = 0; i < features.size(); i++)//遍历layer上的所有要素
	{
		GeoFeature* feature = features.at(i);
		QList<QString> keys = feature->getAttributeMap()->keys();
		QList<QVariant> values = feature->getAttributeMap()->values();
		for (int j = 0; j < keys.size(); j++)
		{
			if (keys.at(j) == attriName)//如果是要查找的属性名
			{
				if (values.at(j).toString().contains(attriValue))//如果输入的属性值是要素属性值的一部分
				{
					featuresFound.append(feature);
				}
			}
		}
	}
	return featuresFound;
}

QList<QString> GeoLayer::getAttriNames()
{
	QVariantMap *variantMap=this->getFeatureAt(0)->getAttributeMap();
	return variantMap->keys();
}

void GeoLayer::setIndexMode(int mode)
{
	indexMode = mode;
}

int GeoLayer::getIndexMode()
{
	return indexMode;
}

void GeoLayer::setSpatialIndex(Index *idx)
{
	spatialIndex = idx;
}
	
QRectF GeoLayer::getRect()
{
	if(!size()){
		return QRectF(0,0,0,0);   //无效结果
	}
	float left,top,right,bottom;
	if(type == EnumType::POINT){  //点图层point
		GeoPoint* firstPoint = (GeoPoint*)(features.at(0)->getGeometry());
		 left = firstPoint->getXf();
		 top = firstPoint->getYf();
		 right = firstPoint->getXf();
		 bottom = firstPoint->getYf();

		for(int i = 0; i < size(); i++){
			GeoPoint* p = (GeoPoint*)features.at(i)->getGeometry();
			if(p->getXf() < left) left = p->getXf();
			if(p->getYf() > top) top = p->getYf();
			if(p->getXf() > right) right = p->getXf();
			if(p->getYf() < bottom) bottom = p->getYf();
		}
	}else if(type == EnumType::POLYLINE){ //线图层polyline
		GeoPolyline* firstPolyline = (GeoPolyline*)(features.at(0)->getGeometry());
		GeoPoint* firstPoint = firstPolyline->getPointAt(0);
		 left = firstPoint->getXf();
		 top = firstPoint->getYf();
		 right = firstPoint->getXf();
		 bottom = firstPoint->getYf();

		for(int i = 0; i < size(); i++){
			QRectF rect = ((GeoPolyline*)features.at(i)->getGeometry())->getRect();
			if(rect.left() < left) left = rect.left();
			if(rect.top() > top) top = rect.top();
			if(rect.right() > right) right = rect.right();
			if(rect.bottom() < bottom) bottom = rect.bottom();
		}
	}else if(type == EnumType::POLYGON){ //面图层polygon
		GeoPolygon* firstPolyline = (GeoPolygon*)features.at(0)->getGeometry();
		GeoPoint* firstPoint = firstPolyline->getPointAt(0);
		 left = firstPoint->getXf();
		 top = firstPoint->getYf();
		 right = firstPoint->getXf();
		 bottom = firstPoint->getYf();

		for(int i = 0; i < size(); i++){
			QRectF rect = ((GeoPolygon*)features.at(i)->getGeometry())->getRect();
			if(rect.left() < left) left = rect.left();
			if(rect.top() > top) top = rect.top();
			if(rect.right() > right) right = rect.right();
			if(rect.bottom() < bottom) bottom = rect.bottom();
		}
	}
	return QRectF(QPointF(left,top),QPointF(right,bottom));
}

bool GeoLayer::isVisable()
{
	return this->visibility;
}

void GeoLayer::setVisable(bool visibility)
{
	this->visibility = visibility;
}

QString GeoLayer::getFullPath()
{
	return this->fullpath;
}

void GeoLayer::setFullPath(QString fullpath)
{
	this->fullpath = fullpath;
}

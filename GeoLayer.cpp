#include "geolayer.h"
#include "EnumType.h"

GeoLayer::GeoLayer():render(NULL),visibility(true)
{
	type = -1;
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
	features.clear();
	return features;
}

void GeoLayer::setRender(Render * render)
{
	this->render = render;
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
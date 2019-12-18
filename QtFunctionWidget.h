#pragma once
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include "GeoMap.h"
#include "GeoLayer.h"
#include <QMouseEvent>
#include <qstring.h>

//测试核密度时加入的
#include"kernalDensity.h"
#include"GeoTool.h"
#include"CGeoKernalDensTool.h"

class QtFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    QtFunctionWidget(QWidget *parent = nullptr);
    ~QtFunctionWidget() Q_DECL_OVERRIDE;
	bool isExist(GeoLayer* layer);
	bool isExist(QString fullpath);
	void strongUpdata(); //开放给父类使用，调用project和update，对应
	void renderLayer(GeoLayer* layer);   //数据和renderer发生了变化的时候调用

protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;  //调用update的时候不保证调用resizeGL函数！！因此project函数需要手动调用
    virtual void paintGL() Q_DECL_OVERRIDE;

private:
	
	GeoMap* map;
    QOpenGLShaderProgram shaderProgram;
	QMap<GeoFeature*, QList<QOpenGLVertexArrayObject*>*> featureVaosMap;
	QMap<GeoLayer*, QList<QOpenGLBuffer*>*> layerBosMap;

	GeoLayer* currentLayer; //储存当前图层，对影响,用于显示与操作
	GeoLayer* tempProcessLayer; //用于处理数据读入配置的中转图层
	QList<GeoLayer*> waitLoadedLayers;

	int w;
	int h;
	QRectF worldRect;
	QRectF originWorldRect;
	bool hasSetRect;  //表示是否为第一次或从无到有加载
	bool hasWaH;     //表是否h与w已经初始化过一次
	QMatrix4x4 view;
	QMatrix4x4 projection;
	QPoint screenPointBegin;
	QPoint screenPointDuring;
	QPoint screenPointEnd;
	float scale;

	//-------------------之间的函数会保证安全性，不用额外判断---------------------
	void addlayer(GeoLayer* layer);
	void changeLayer(GeoLayer* layer);
	void changeLayer(QString fullpath);   //change指改变正在进行空间操作的图层
	GeoLayer* removeLayer(GeoLayer* layer);
	GeoLayer* removeLayer(QString fullpath);
	void deleteLayer(GeoLayer* layer);
	void initLayer(GeoLayer* layer);
	void initLayer(QString fullpath);
	void switchLayer(GeoLayer* layer);    //切换current图层，不执行重绘
	void switchLayer(QString fullpath);
	void switchWorldRect(GeoLayer* layer);		//切换绘制区域，执行重绘
	void switchWorldRect(QString fullpath);
   //------------------------------------------------------------------------

	void releaseVaos(GeoLayer* layer);
	void bindVaos(GeoLayer* layer);
	void project();   //使用世界窗口进行投影
	void loadWaitLayers();
	void refreshWorldRectForTrans(QPoint begin, QPoint end);
	void refreshWorldRectForScale(QPoint originScreen, float scale);
	QPointF screenToWorld(QPoint screenPoint);
	QPointF screenToNormalizedPos(QPoint screenPoint);
	void initWorldRect(GeoLayer* layer);  //初始化layer的世界窗口大小，保证zoom的图层在屏幕中央
	int getResizeDirection(QRect oriRect,QRect newRect);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

public slots:
	void on_addLayerData(GeoLayer* layer);
	void on_deleteLayerData(GeoLayer* layer);
	void on_zoomToLayerRect(GeoLayer* layer);
	void on_setSymbol(Symbol* symbol);
	//测试核密度
signals:

};


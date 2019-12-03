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

class QtFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    QtFunctionWidget(QWidget *parent = nullptr);
    ~QtFunctionWidget() Q_DECL_OVERRIDE;

	void addlayer(GeoLayer* layer);
	void changeLayer(GeoLayer* layer);
	void changeLayer(QString name);   //change指改变正在进行空间操作的图层
	void removeLayer(GeoLayer* layer);
	void removeLayer(QString name);
	bool isExist(GeoLayer* layer);
	bool isExist(QString* name);

protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;

private:
	
	GeoMap* map;
    QOpenGLShaderProgram shaderProgram;
    QMap<GeoFeature*,QOpenGLBuffer*>  featureVbosMap;
	QMap<GeoFeature*,QOpenGLVertexArrayObject*> featureVaosMap;

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

	void initLayer(GeoLayer* layer);
	void initLayer(QString fullpath);
	void switchLayer(GeoLayer* layer);    //切换current图层，不执行重绘
	void switchLayer(QString fullpath);
	void switchWorldRect(GeoLayer* layer);		//切换绘制区域，执行重绘
	void switchWorldRect(QString fullpath);
	void bindVao();
	void project();   //使用世界窗口进行投影
	void loadWaitLayers();
	QPointF screenToWorld(QPoint screenPoint);
	void refreshWorldRectForTrans(QPoint begin, QPoint end);
	void refreshWorldRectForScale(QPoint originScreen, float scale);
	QPointF screenToNormalizedPos(QPoint screenPoint);
	void initWorldRect(GeoLayer* layer);  //初始化layer的世界窗口大小，保证zoom的图层在屏幕中央
	int getResizeDirection(QRect oriRect,QRect newRect);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
public slots:
	void on_deleteLayerData(const QString& fullpath);
	void on_zoomToLayerRect(const QString& fullpath);


};


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
	void changeLayer(QString name);   //changeָ�ı����ڽ��пռ������ͼ��
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

	GeoLayer* currentLayer; //���浱ǰͼ�㣬��Ӱ��,������ʾ�����
	GeoLayer* tempProcessLayer; //���ڴ������ݶ������õ���תͼ��
	QList<GeoLayer*> waitLoadedLayers;

	int w;
	int h;
	QRectF worldRect;
	QRectF originWorldRect;
	bool hasSetRect;  //��ʾ�Ƿ�Ϊ��һ�λ���޵��м���
	bool hasWaH;     //���Ƿ�h��w�Ѿ���ʼ����һ��
	QMatrix4x4 view;
	QMatrix4x4 projection;
	QPoint screenPointBegin;
	QPoint screenPointDuring;
	QPoint screenPointEnd;
	float scale;

	void initLayer(GeoLayer* layer);
	void initLayer(QString fullpath);
	void switchLayer(GeoLayer* layer);    //�л�currentͼ�㣬��ִ���ػ�
	void switchLayer(QString fullpath);
	void switchWorldRect(GeoLayer* layer);		//�л���������ִ���ػ�
	void switchWorldRect(QString fullpath);
	void bindVao();
	void project();   //ʹ�����細�ڽ���ͶӰ
	void loadWaitLayers();
	QPointF screenToWorld(QPoint screenPoint);
	void refreshWorldRectForTrans(QPoint begin, QPoint end);
	void refreshWorldRectForScale(QPoint originScreen, float scale);
	QPointF screenToNormalizedPos(QPoint screenPoint);
	void initWorldRect(GeoLayer* layer);  //��ʼ��layer�����細�ڴ�С����֤zoom��ͼ������Ļ����
	int getResizeDirection(QRect oriRect,QRect newRect);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
public slots:
	void on_deleteLayerData(const QString& fullpath);
	void on_zoomToLayerRect(const QString& fullpath);


};


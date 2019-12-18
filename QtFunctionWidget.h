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

//���Ժ��ܶ�ʱ�����
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
	void strongUpdata(); //���Ÿ�����ʹ�ã�����project��update����Ӧ
	void renderLayer(GeoLayer* layer);   //���ݺ�renderer�����˱仯��ʱ�����

protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;  //����update��ʱ�򲻱�֤����resizeGL�����������project������Ҫ�ֶ�����
    virtual void paintGL() Q_DECL_OVERRIDE;

private:
	
	GeoMap* map;
    QOpenGLShaderProgram shaderProgram;
	QMap<GeoFeature*, QList<QOpenGLVertexArrayObject*>*> featureVaosMap;
	QMap<GeoLayer*, QList<QOpenGLBuffer*>*> layerBosMap;

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

	//-------------------֮��ĺ����ᱣ֤��ȫ�ԣ����ö����ж�---------------------
	void addlayer(GeoLayer* layer);
	void changeLayer(GeoLayer* layer);
	void changeLayer(QString fullpath);   //changeָ�ı����ڽ��пռ������ͼ��
	GeoLayer* removeLayer(GeoLayer* layer);
	GeoLayer* removeLayer(QString fullpath);
	void deleteLayer(GeoLayer* layer);
	void initLayer(GeoLayer* layer);
	void initLayer(QString fullpath);
	void switchLayer(GeoLayer* layer);    //�л�currentͼ�㣬��ִ���ػ�
	void switchLayer(QString fullpath);
	void switchWorldRect(GeoLayer* layer);		//�л���������ִ���ػ�
	void switchWorldRect(QString fullpath);
   //------------------------------------------------------------------------

	void releaseVaos(GeoLayer* layer);
	void bindVaos(GeoLayer* layer);
	void project();   //ʹ�����細�ڽ���ͶӰ
	void loadWaitLayers();
	void refreshWorldRectForTrans(QPoint begin, QPoint end);
	void refreshWorldRectForScale(QPoint originScreen, float scale);
	QPointF screenToWorld(QPoint screenPoint);
	QPointF screenToNormalizedPos(QPoint screenPoint);
	void initWorldRect(GeoLayer* layer);  //��ʼ��layer�����細�ڴ�С����֤zoom��ͼ������Ļ����
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
	//���Ժ��ܶ�
signals:

};


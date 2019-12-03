#include "QtFunctionWidget.h"
#include "EnumType.h"
#include <QFile>
#include <cmath>
#include "util.h"

QtFunctionWidget::QtFunctionWidget(QWidget *parent) : QOpenGLWidget (parent)
		,hasSetRect(false), scale(0.8), hasWaH(false),currentLayer(NULL)
{
	map = new GeoMap;
	setMouseTracking(true);
}

QtFunctionWidget::~QtFunctionWidget(){
    makeCurrent();
	
    shaderProgram.release();
    doneCurrent();
	for(int i = 0; i < map->size();i++){
		GeoLayer* layer = map->getLayerAt(i);
		for(int j = 0; j < layer->size();j++){
			QOpenGLVertexArrayObject* vao = featureVaosMap[layer->getFeatureAt(j)];
			vao->destroy();
			delete vao;
		}
	}
	for(int i = 0; i < map->size();i++){
		GeoLayer* layer = map->getLayerAt(i);
		for(int j = 0; j < layer->size();j++){
			QOpenGLBuffer* vbo = featureVbosMap[layer->getFeatureAt(j)];
			vbo->destroy();
			delete vbo;
		}
	}
	
	//�����map���Է�ֹ������objΪNULL
	if(map){
		delete map;
	}
}

void QtFunctionWidget::initializeGL(){
	if(!map){
		map = new GeoMap;
	}
    this->initializeOpenGLFunctions();

    bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "GLConfig//vertexShader1.vsh");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "GLConfig//fragmentShader1.fsh");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.link();
    if(!success) {
        qDebug() << "shaderProgram link failed!" << shaderProgram.log();
    }
}

void QtFunctionWidget::resizeGL(int w, int h){
	
	glViewport(0, 0, w, h);
	if(currentLayer == NULL){
		return;
	}
	if (!hasWaH) {
		this->w = w;
		this->h = h;
		hasWaH = true;
		return;
	}
	QPointF center = worldRect.center();
	float width = worldRect.width() * w * 1.0 / this->w;
	float height = worldRect.height() * h * 1.0 / this->h;
	worldRect = QRectF(QPoint(center.x() - width / 2, center.y() - height / 2)
		, QPoint(center.x() + width / 2, center.y() + height / 2));
	this->w = w;
	this->h = h;
    project();    //����ͶӰ
}



void QtFunctionWidget::paintGL(){
	loadWaitLayers();   //���еĹ���gl������߼��ڴ�ִ��
	if(!map->size()){
		return;
	}
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.bind();
    {
		qDebug() << "........................................";
		for(int j = 0; j < map->size();j++){
			GeoLayer* layer = map->getLayerAt(j);
			if(layer->isVisable()){
				for(int i = 0; i < layer->size();i++){
					 QOpenGLVertexArrayObject::Binder vaoBind(featureVaosMap[layer->getFeatureAt(i)]);
					 glDrawArrays(GL_LINE_LOOP,0,layer->getFeatureAt(i)->getGeometry()->size());
				}
			}
		}
    }
	shaderProgram.release();
}



//�������⿪�ţ�ֻ������ȴ���������Ӵ�����ͼ�㣬�Ӷ�ʵ���첽����
void QtFunctionWidget::addlayer(GeoLayer* layer)
{
	waitLoadedLayers.push_back(layer);
}
void QtFunctionWidget::changeLayer(GeoLayer* layer)
{
	waitLoadedLayers.push_back(layer);
}
void QtFunctionWidget::changeLayer(QString name)
{
	for(int i = 0; i < map->size(); i++){
		if(name == map->getLayerAt(i)->getName()){
			waitLoadedLayers.push_back(map->getLayerAt(i));
		}
	}
}
void QtFunctionWidget::removeLayer(GeoLayer* layer)
{
	for(int i = 0; i < map->size(); i++){
		if(layer == map->getLayerAt(i)){
			map->removeLayerAt(i);
			if(currentLayer == layer){
				if(map->size()){
					currentLayer = map->getLayerAt(0);
				}else{
				    currentLayer = NULL;
					hasSetRect = false;
				}
			}
			delete layer;
			break;
		}
	}
}
void QtFunctionWidget::removeLayer(QString name)
{
	for(int i = 0; i < map->size(); i++){
		if(name == map->getLayerAt(i)->getName()){
			map->removeLayerAt(i);
			if(currentLayer == map->getLayerAt(i)){
				if(map->size()){
					currentLayer = map->getLayerAt(0);
				}else{
				    currentLayer = NULL;
					hasSetRect = false;
				}
			}
			delete map->getLayerAt(i);
			break;
		}
	}
}
bool QtFunctionWidget::isExist(GeoLayer* layer)
{
	for(int i = 0; i < map->size(); i++){
		if(layer == map->getLayerAt(i)){
			return true;
		}
	}
	return false;
}
bool QtFunctionWidget::isExist(QString* name)
{
	for(int i = 0; i < map->size(); i++){
		if(name == map->getLayerAt(i)->getName()){
			return true;
		}
	}
	return false;
}

//��gl��ͼ��صĺ���������init��resize��paintgl�б����ã�����֮�ⲻ����أ������Ҫ�����ݼ��غͻ�ͼ���룬���첽�ķ�ʽ���������������
void QtFunctionWidget::initLayer(GeoLayer* layer)
{
	if(!isExist(layer)){
		map->addLayer(layer);
		tempProcessLayer = layer;
		bindVao();
	}
	if(!hasSetRect){   //��һ������
		switchLayer(layer);
		hasSetRect = true;
	}
}

void QtFunctionWidget::initLayer(QString fullpath)
{
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	initLayer(layer);
}
void QtFunctionWidget::switchLayer(GeoLayer* layer)
{
	if(isExist(layer)){
		currentLayer = layer;
		switchWorldRect(layer);
	}
}
void QtFunctionWidget::switchLayer(QString fullpath)
{
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	switchLayer(layer);
}
void QtFunctionWidget::switchWorldRect(GeoLayer* layer)
{
	initWorldRect(layer);
	project();
	update();
}
void QtFunctionWidget::switchWorldRect(QString fullpath)
{
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	switchWorldRect(layer);
}

//������ش�����ͼ�㣬����paintGL�б�����
void QtFunctionWidget::loadWaitLayers()
{
	for(int i = 0; i < waitLoadedLayers.size();i++){
		initLayer(waitLoadedLayers.at(i));
	}
	waitLoadedLayers.clear();
}


void QtFunctionWidget::bindVao()
{
	for(int i = 0;i < tempProcessLayer->size();i++){
		GeoFeature* feature = tempProcessLayer->getFeatureAt(i);
		GeoGeometry* geometry = feature->getGeometry();
		//���ٿռ䲢�����¼
		GLfloat* vertices = (GLfloat*)malloc(sizeof(vertices)*(geometry->size())*3);
		if(geometry->getType() == EnumType::POINT){
			GeoPoint* point = (GeoPoint*)geometry;
			float x = point->getXf();
			float y = point->getYf();
			vertices[0] = GLfloat(x);
			vertices[1] = GLfloat(y);
			vertices[2] = GLfloat(0.0);
		}else if(geometry->getType() == EnumType::POLYLINE){
			GeoPolyline* polyline = (GeoPolyline*)geometry;
			for(int j = 0; j < geometry->size();j++){	
				GeoPoint* point = polyline->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j*3+0] = GLfloat(x);
				vertices[j*3+1] = GLfloat(y);
				vertices[j*3+2] = GLfloat(0.0);
			}
		}else if(geometry->getType() == EnumType::POLYGON){
			GeoPolygon* polygon = (GeoPolygon*)geometry;
			for(int j = 0; j < geometry->size();j++){	
				GeoPoint* point = polygon->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j*3+0] = GLfloat(x);
				vertices[j*3+1] = GLfloat(y);
				vertices[j*3+2] = GLfloat(0.0);
			}
		}


		QOpenGLVertexArrayObject* vao = new QOpenGLVertexArrayObject();
		featureVaosMap.insert(feature,vao);
		QOpenGLVertexArrayObject::Binder vaoBind(vao);

	
		QOpenGLBuffer* vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		featureVbosMap.insert(feature,vbo);
		vbo->create();
		vbo->bind();
		vbo->allocate(vertices, sizeof(vertices)*(geometry->size())*3);

		int posAttr = -1;
		//int colAttr = -1;
		//ͨ���������Ӷ�����ɫ���л�ȡ����Ӧ������location
		posAttr = shaderProgram.attributeLocation("aPos");
		//colAttr = shaderProgram.attributeLocation("aColor");
	
		shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * 3);
		//shaderProgram.setAttributeBuffer(colAttr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
	
		shaderProgram.enableAttributeArray(posAttr);
	   // shaderProgram.enableAttributeArray(colAttr);
	
		shaderProgram.release();
		vbo->release();
	}
}

void QtFunctionWidget::project()
{
	//����ͶӰ
	shaderProgram.bind();   // don't forget to activate/use the shader before setting uniforms!
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	view = QMatrix4x4();
	view.translate(QVector3D(0.0f, 0.0f, -3.0f));
	shaderProgram.setUniformValue("view", view);
	projection = QMatrix4x4();
	
	float left = worldRect.left();
	float right = worldRect.right();
	float bottom = worldRect.bottom();
	float top = worldRect.top();
	projection.ortho(left, right, bottom, top, 0.1f, 100.0f);
	shaderProgram.setUniformValue("projection", projection);

	shaderProgram.release();
}

void QtFunctionWidget::on_deleteLayerData(const QString& fullpath){
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	for(int i = 0; i < layer->size();i++){
		GeoFeature* feature = layer->getFeatureAt(i);
		GeoGeometry* obj = feature->getGeometry();
		QOpenGLBuffer* vbo = featureVbosMap[feature];
		featureVaosMap.remove(feature);
		delete vbo;
		QOpenGLVertexArrayObject* vao = featureVaosMap[feature];
		featureVaosMap.remove(feature);
		delete vao;
	}
	delete map->remove(layer);
	if (!map->size()) {
		hasSetRect = false;
		hasWaH = false;
	} 
	update();
}

void QtFunctionWidget::on_zoomToLayerRect(const QString& fullpath)
{
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	switchLayer(layer);
}

void QtFunctionWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton){   //�Ҽ�
		
	}else if(e->button() == Qt::RightButton){   //���
       
	}else if(e->button() == Qt::MidButton){   //���ֵ���
		//��ʼ��
		screenPointBegin = e->pos();
		screenPointDuring = e->pos();
		screenPointEnd = e->pos();
		originWorldRect = worldRect;
	}
}

void QtFunctionWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(e->buttons() & Qt::LeftButton){ //����ƶ�����ס���
       
	}else if(e->buttons() & Qt::RightButton){   //����ƶ�����ס�Ҽ�
	
	}else if(e->buttons() & Qt::MidButton){   //����ƶ�����ס���� ----  ��ͼ�ƶ�
		//�����м������
		screenPointDuring = e->pos();
		refreshWorldRectForTrans(screenPointBegin, screenPointDuring);
		project();
		update();
	}
}

void QtFunctionWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {   //�Ҽ�

	}
	else if (e->button() == Qt::RightButton) {   //���

	}
	else if (e->button() == Qt::MidButton) {   //���ֵ���
		//���½���������
		screenPointEnd = e->pos();
		refreshWorldRectForTrans(screenPointBegin, screenPointEnd);
		project();
		update();
	}
}

void QtFunctionWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() > 0){   //Զ���ң���ǰ����----��ͼ��С
		refreshWorldRectForScale(e->pos(), 1.0 / scale);
		project();
		update();
	}else if(e->delta() < 0) {      //�����ң�������----��ͼ�Ŵ�
		refreshWorldRectForScale(e->pos(), scale);
		project();
		update();
	}
}

void QtFunctionWidget::refreshWorldRectForTrans(QPoint begin, QPoint end)
{
	QPointF worldPointBegin = screenToWorld(begin);
	QPointF worldPointEnd = screenToWorld(end);
	QPointF moveVector = worldPointEnd - worldPointBegin;
	QPointF leftTopPoint = originWorldRect.topLeft();
	QPointF rightBottomPoint = originWorldRect.bottomRight();
	worldRect = QRectF(leftTopPoint - moveVector, rightBottomPoint - moveVector);
}

void QtFunctionWidget::refreshWorldRectForScale(QPoint originScreen, float scale)
{
	qDebug() << "center"<< originScreen;
	QPointF originWorld = screenToWorld(originScreen);
	qDebug() << "origin rect" << worldRect;
	qDebug() << "origin cneter" << originWorld;
	QPointF leftTop = worldRect.topLeft();
	qDebug() << "origin lefttop" << leftTop;
	QPointF rightBottom = worldRect.bottomRight();
	qDebug() << "origin rightBottom" << rightBottom;
	QPointF newLeftTop = (leftTop - originWorld)*scale + originWorld;
	qDebug() << "new lefttop" << newLeftTop;
	QPointF newRightBottom = (rightBottom - originWorld)*scale + originWorld;
	qDebug() << "new rightBottom" << newRightBottom;
	worldRect = QRectF(newLeftTop, newRightBottom);
	qDebug() << "�������" << worldRect;
}


QPointF QtFunctionWidget::screenToWorld(QPoint screenPoint)
{
	QPointF normalizedPoint = screenToNormalizedPos(screenPoint);
	QVector4D normalizedPoint4D(normalizedPoint);
	QVector4D worldPoint4D = view.inverted()*projection.inverted()*normalizedPoint4D;
	QPointF worldCenter = worldRect.center();
	return worldPoint4D.toPointF() + worldCenter;
}

QPointF QtFunctionWidget::screenToNormalizedPos(QPoint screenPoint)
{
	QPointF normalizedPoint;
	double w = this->width() * 1.0;
	double h = this->height() * 1.0;
	double x = screenPoint.x() * 1.0;
	double y = screenPoint.y() * 1.0;

	normalizedPoint.setX((2 * x / w) - 1);
	normalizedPoint.setY(1 - (2 * y / h));

	return normalizedPoint;
}

void QtFunctionWidget::initWorldRect(GeoLayer* layer)
{
	QRectF layerRect = layer->getRect();
	QPointF center = layerRect.center();
	int layerWidth = layerRect.width();
	int layerHeight = layerRect.height();
	float layerRadio = layerWidth * 1.0 / layerHeight;
	float viewRadio = width() * 1.0 / height();

	//�������ݱ�
	if (abs(layerRadio) > abs(viewRadio)) {
		layerHeight = -abs(layerWidth / viewRadio);
	}
	else {
		layerWidth = abs(layerHeight*viewRadio);
	}
	QPoint leftTop(center.x() - layerWidth * 1.0 / 2, center.y() - layerHeight * 1.0 / 2);
	worldRect = QRect(leftTop, QSize(layerWidth, layerHeight));
}

int QtFunctionWidget::getResizeDirection(QRect oriRect, QRect newRect)
{
	int oleft = oriRect.left();
	int oright = oriRect.right();
	int otop = oriRect.top();
	int obottom = oriRect.bottom(); 
	int nleft = newRect.left();
	int nright = newRect.right();
	int ntop = newRect.top();
	int nbottom = newRect.bottom();

	if (oleft != nleft && otop != ntop) {
		return EnumType::LEFTTOP;
	}else if (oright != nright && otop != ntop) {
		return EnumType::RIGHTTOP;
	}else if (oleft != nleft && obottom != nbottom) {
		return EnumType::LRFTBOTTOM;
	}else if (oright != nright && obottom != nbottom) {
		return EnumType::RIGHTBOTTOM;
	}
	else if (oleft != nleft) {
		return EnumType::LEFT;
	}
	else if (otop != ntop) {
		return EnumType::TOP;
	}
	else if (oright != nright) {
		return EnumType::RIGHT;
	}
	else if (obottom != nbottom) {
		return EnumType::BOTTOM;
	}
	return EnumType::NOCHANGE;
}

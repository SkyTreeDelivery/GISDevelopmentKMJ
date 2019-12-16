#include "QtFunctionWidget.h"
#include "EnumType.h"
#include <QFile>
#include <cmath>
#include "util.h"
#include <qlist.h>

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
		deleteLayer(map->getLayerAt(i));
	}
	
	//最后处理map，以防止关联的obj为NULL
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
	worldRect = QRectF(QPointF(center.x() - width / 2, center.y() - height / 2)
		, QPointF(center.x() + width / 2, center.y() + height / 2));
	this->w = w;
	this->h = h;
    project();    //更新投影
}



void QtFunctionWidget::paintGL(){
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	loadWaitLayers();   //所有的关于gl的添加逻辑在此执行
	if(!map->size()){
		return;
	}
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.bind();
    {
		qDebug() << "........................................";
		for(int j = 0; j < map->size();j++){
			GeoLayer* layer = map->getLayerAt(j);
			if(layer->isVisable()){
				int type = layer->getType();
				if (type == EnumType::POINT) glLineWidth(layer->getRender()->getMarkerSymbol()->getOutline()->getWidth());
				else if(type == EnumType::POLYLINE) glLineWidth(layer->getRender()->getLineSymbol()->getWidth());
				else if (type = EnumType::POLYGON) glLineWidth(layer->getRender()->getFillSymbol()->getOutline()->getWidth());
				for(int i = 0; i < layer->size();i++){
					GeoFeature* feature = layer->getFeatureAt(i);
					QList<QOpenGLVertexArrayObject*>* vaos = featureVaosMap[feature];
					for (int j = 0; j < vaos->size(); j++) {
						QOpenGLVertexArrayObject* vao = vaos->at(j);
						QOpenGLVertexArrayObject::Binder vaoBind(vao);
						if (vao->property("type").toInt() == EnumType::bufferType::VBO) {
							glDrawArrays(GL_LINE_LOOP, 0, layer->getFeatureAt(i)->getGeometry()->size());
						}
						else if (vao->property("type").toInt() == EnumType::bufferType::EBO) {
							glDrawElements(GL_TRIANGLES, vao->property("indexNum").toInt(), GL_UNSIGNED_INT, 0);
						}
					}
				}
			}
		}
    }
	shaderProgram.release();
}



//函数对外开放，只负责向等待队列中添加待加载图层，从而实现异步加载
void QtFunctionWidget::addlayer(GeoLayer* layer)
{
	if (!isExist(layer)) {
		waitLoadedLayers.push_back(layer);
	}
}
void QtFunctionWidget::renderLayer(GeoLayer * layer)
{
	if (layer) {
		layer->setWaitingRendered(true);
		waitLoadedLayers.push_back(layer);
	}
}

void QtFunctionWidget::changeLayer(GeoLayer* layer)
{
	if (isExist(layer)) {
		waitLoadedLayers.push_back(layer);
	}
}
void QtFunctionWidget::changeLayer(QString fullpath)
{
	changeLayer(map->getLayerByFullpath(fullpath));
}

GeoLayer* QtFunctionWidget::removeLayer(GeoLayer* layer)
{
	if (isExist(layer)) {
		releaseVaos(layer);
		if (!map->size()) {
			hasSetRect = false;
			hasWaH = false;
		}
		return map->remove(layer);
	}
}
GeoLayer* QtFunctionWidget::removeLayer(QString fullpath)
{
	return removeLayer(map->getLayerByFullpath(fullpath));
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
bool QtFunctionWidget::isExist(QString fullpath)
{
	for(int i = 0; i < map->size(); i++){
		if(fullpath == map->getLayerAt(i)->getFullPath()){
			return true;
		}
	}
	return false;
}

void QtFunctionWidget::strongUpdata()
{
	project();
	update();
}

//note that ：不要使initLayer在其他的代码中被显示调用，如想对图层进行操作，请使用addlayer和deletelayer ！！！！！原因如下
//与gl绘图相关的函数必须在init，resize和paintgl中被调用，除此之外不会加载，因此需要将数据加载和绘图分离，以异步的方式完成整个加载流程
//如果你想知道在这三个函数之外调用回发生什么，请自己尝试，反正我debug了很久踩的坑。在使用bindVaos函数的时候，其他一切正常，但是对于内存中储存的数据造成了破坏，具体原因未知
void QtFunctionWidget::initLayer(GeoLayer* layer)
{
	if(!isExist(layer)){
		map->addLayer(layer);
		tempProcessLayer = layer;
		bindVaos(layer);
	}
	else {
		if (layer->isWaitingRendered()) {
			releaseVaos(layer);
			bindVaos(layer);
			project();    //数据可能发生了变化，因此调用一次，这里不适用update()
		}
	}
	if(!hasSetRect){   //第一次配置
		switchLayer(layer);
		switchWorldRect(layer);
		hasSetRect = true;
	}
}

void QtFunctionWidget::initLayer(QString fullpath)
{
	initLayer(map->getLayerByFullpath(fullpath));
}
void QtFunctionWidget::switchLayer(GeoLayer* layer)
{
	if(isExist(layer)){
		currentLayer = layer;
	}
}
void QtFunctionWidget::switchLayer(QString fullpath)
{
	switchLayer(map->getLayerByFullpath(fullpath));
}
void QtFunctionWidget::switchWorldRect(GeoLayer* layer)
{
	if (isExist(layer)) {
		initWorldRect(layer);
		project();
		update();
	}
}
void QtFunctionWidget::switchWorldRect(QString fullpath)
{
	GeoLayer* layer = map->getLayerByFullpath(fullpath);
	switchWorldRect(layer);
}

//负责加载代价在图层，会在paintGL中被调用
void QtFunctionWidget::loadWaitLayers()
{
	for(int i = 0; i < waitLoadedLayers.size();i++){
		initLayer(waitLoadedLayers.at(i));
	}
	waitLoadedLayers.clear();
}


void QtFunctionWidget::bindVaos(GeoLayer* layer)
{
	QList<QOpenGLBuffer*>* boList = new QList<QOpenGLBuffer*>;
	layerBosMap.insert(layer, boList);
	for(int i = 0;i < tempProcessLayer->size();i++){
		GeoFeature* feature = layer->getFeatureAt(i);
		GeoGeometry* geometry = feature->getGeometry();
		//开辟空间并保存记录
		int size = 6;
		if (geometry->getType() == EnumType::POINT) {
			GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			MarkerSymbol* markerSymbol = layer->getRender()->getMarkerSymbol();
			QColor color = markerSymbol->getColor();
			GeoPoint* point = (GeoPoint*)geometry;
			float x = point->getXf();
			float y = point->getYf();
			vertices[0] = GLfloat(x);
			vertices[1] = GLfloat(y);
			vertices[2] = GLfloat(0.0);
			vertices[3] = color.red()*1.0/255;
			vertices[4] = color.green()*1.0/255;
			vertices[5] = color.blue()*1.0/255;

			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			QOpenGLVertexArrayObject* vao = new QOpenGLVertexArrayObject();
			vaoList->push_back(vao);
			vao->setProperty("type", QVariant(EnumType::bufferType::VBO));
			featureVaosMap.insert(feature, vaoList);
			
			QOpenGLVertexArrayObject::Binder vaoBind0(vao);
			QOpenGLBuffer* vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			vbo->create();
			vbo->bind();
			vbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);
			
			boList->push_back(vbo);

			int posAttr = -1;
			int colAttr = -1;
			//通过属性名从顶点着色器中获取到相应的属性location
			posAttr = shaderProgram.attributeLocation("aPos");
			colAttr = shaderProgram.attributeLocation("aColor");

			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.setAttributeBuffer(colAttr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * size);

			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.enableAttributeArray(colAttr);

			shaderProgram.release();

			vbo->release();
		}
		else if (geometry->getType() == EnumType::POLYLINE) {
			GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			GeoPolyline* polyline = (GeoPolyline*)geometry;
			LineSymbol* lineSymbol = layer->getRender()->getLineSymbol();
			QColor color = lineSymbol->getColor();
			for (int j = 0; j < geometry->size(); j++) {
				GeoPoint* point = polyline->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j * size + 0] = GLfloat(x);
				vertices[j * size + 1] = GLfloat(y);
				vertices[j * size + 2] = GLfloat(0.0);
				vertices[j * size + 3] = color.red()*1.0 / 255;
				vertices[j * size + 4] = color.green()*1.0 / 255;
				vertices[j * size + 5] = color.blue()*1.0 * 255;
			}

			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			QOpenGLVertexArrayObject* vao = new QOpenGLVertexArrayObject();
			vaoList->push_back(vao);
			vao->setProperty("type", QVariant(EnumType::bufferType::VBO));
			featureVaosMap.insert(feature, vaoList);

			QOpenGLVertexArrayObject::Binder vaoBind0(vao);
			QOpenGLBuffer* vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			vbo->create();
			vbo->bind();
			vbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);

			boList->push_back(vbo);

			int posAttr = -1;
			int colAttr = -1;
			//通过属性名从顶点着色器中获取到相应的属性location
			posAttr = shaderProgram.attributeLocation("aPos");
			colAttr = shaderProgram.attributeLocation("aColor");

			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.setAttributeBuffer(colAttr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * size);

			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.enableAttributeArray(colAttr);

			shaderProgram.release();

			vbo->release();
		}
		else if (geometry->getType() == EnumType::POLYGON) {
			//公用部分
			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			featureVaosMap.insert(feature, vaoList);
			GeoPolygon* polygon = (GeoPolygon*)geometry;
			GLfloat* vertices = NULL;

			//剖分三角
			FillSymbol* fillSymbol = layer->getRender()->getFillSymbol();
			QColor fillColor = fillSymbol->getColor();

			//三角剖分数据 - level0
			gpc_tristrip tristrip;
			util::tesselation(polygon, &tristrip); 

			int stripNum = tristrip.num_strips;		//三角分组数
			int triNum = 0;		//三角个数
			int vertexNum = 0;		//顶点个数，没有重复顶点
			for (int i = 0; i < stripNum; i++)
			{
				triNum += tristrip.strip[i].num_vertices - 2;//计算一共有多少三角形
				vertexNum += tristrip.strip[i].num_vertices;//计算一共有多少顶点
			}

			//创建顶点数组
			vertices = new GLfloat[size * vertexNum];
			int idx = 0;
			for (int i = 0; i < stripNum; i++) {  //i为每个三角形组
				gpc_vertex_list list = tristrip.strip[i];
				for (int j = 0; j < list.num_vertices; j++) {  //j为三角形组中的每个顶点
					gpc_vertex vertex = list.vertex[j];
					float x = vertex.x;
					float y = vertex.y;
					vertices[idx * size + 0] = GLfloat(x);
					vertices[idx * size + 1] = GLfloat(y);
					vertices[idx * size + 2] = GLfloat(0.0);
					vertices[idx * size + 3] = fillColor.red()*1.0/255;
					vertices[idx * size + 4] = fillColor.green()*1.0/255;
					vertices[idx * size + 5] = fillColor.blue()*1.0/255;
					idx++;
				}
			}

			//创建索引数组
			unsigned int *indices = new unsigned int[triNum * 3];
			int idx1 = 0;
			int cumulatedVertex = 0;
			for (int i = 0; i < stripNum; i++) {//i为每个三角形组
				gpc_vertex_list list = tristrip.strip[i];
				for (int j = 0; j < list.num_vertices - 2; j++) {//j为三角形组中的每个三角形
					for (int k = 0; k < 3; k++) {//k为0-3,代表每个三角形三个顶点的索引
						indices[idx1++] = cumulatedVertex + j + k ;
					}
				}
				cumulatedVertex += list.num_vertices;
			}

			QOpenGLVertexArrayObject* fillVao = new QOpenGLVertexArrayObject();
			vaoList->push_back(fillVao);
			fillVao->setProperty("type", QVariant(EnumType::bufferType::EBO));
			fillVao->setProperty("indexNum",triNum * 3);
			QOpenGLVertexArrayObject::Binder vaoBind0(fillVao);
			QOpenGLBuffer* fillVbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			fillVbo->create();
			fillVbo->bind();
			fillVbo->allocate(vertices, sizeof(GLfloat) * size * vertexNum);
			boList->push_back(fillVbo);

			QOpenGLBuffer* fillEbo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
			fillEbo->create();
			fillEbo->bind();
			fillEbo->allocate(indices, sizeof(unsigned int) * triNum * 3);
			boList->push_back(fillEbo);

			int posAttr = -1;
			int colAttr = -1;
			//通过属性名从顶点着色器中获取到相应的属性location
			posAttr = shaderProgram.attributeLocation("aPos");
			colAttr = shaderProgram.attributeLocation("aColor");

			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.setAttributeBuffer(colAttr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * size);

			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.enableAttributeArray(colAttr);

			shaderProgram.release();
			fillVbo->release();

			/*************************************************************************/

			//边界数据 - level1
			LineSymbol* lineSymbol = layer->getRender()->getLineSymbol();
			QColor lineColor = lineSymbol->getColor();

			vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			for (int j = 0; j < geometry->size(); j++) {
				GeoPoint* point = polygon->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j * size + 0] = GLfloat(x);
				vertices[j * size + 1] = GLfloat(y);
				vertices[j * size + 2] = GLfloat(0.0);
				vertices[j * size + 3] = lineColor.red()*1.0/255;
				vertices[j * size + 4] = lineColor.green()*1.0/255;
				vertices[j * size + 5] = lineColor.blue()*1.0/255;
			}

			QOpenGLVertexArrayObject* outlineVao = new QOpenGLVertexArrayObject();
			outlineVao->setProperty("type", QVariant(EnumType::bufferType::VBO));
			vaoList->push_back(outlineVao);

			QOpenGLVertexArrayObject::Binder vaoBind1(outlineVao);
			QOpenGLBuffer* outlineVbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			outlineVbo->create();
			outlineVbo->bind();
			outlineVbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);

			posAttr = -1;
			colAttr = -1;
			//通过属性名从顶点着色器中获取到相应的属性location
			posAttr = shaderProgram.attributeLocation("aPos");
			colAttr = shaderProgram.attributeLocation("aColor");

			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.setAttributeBuffer(colAttr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * size);

			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.enableAttributeArray(colAttr);

			shaderProgram.release();

			outlineVbo->release();
		}
		else {
			qWarning() << "map class not set";
		}
	}
}

void QtFunctionWidget::project()
{
	//进行投影
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


void QtFunctionWidget::on_addLayerData(GeoLayer* layer) {
	addlayer(layer);
}

void QtFunctionWidget::on_deleteLayerData(GeoLayer* layer){
	deleteLayer(layer);
	update();
}

void QtFunctionWidget::on_zoomToLayerRect(GeoLayer* layer)
{
	switchLayer(layer);
	switchWorldRect(layer);
}

void QtFunctionWidget::on_setSymbol(Symbol * symbol)
{
	Render* render = currentLayer->getRender();
	int type = symbol->getType();
	if (type == EnumType::MARKERSYMBOL) {
		delete render->setMarkerSymbol((MarkerSymbol*)symbol);
	}
	else if (type == EnumType::LINESYMBOL) {
		delete render->setLineSymbol((LineSymbol*)symbol);
	}
	else if (type == EnumType::FILLSYMBOL) {
		delete render->setFillSymbol((FillSymbol*)symbol);
	}
	renderLayer(currentLayer);   //样式发生了变化因此重新渲染
}


void QtFunctionWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton){   //左键
		GeoPoint *point = new GeoPoint;
		point->setXYf(100, 25);
		GeoFeature* feature= map->getLayerAt(0)->Identify(point, map->getLayerAt(0), 0.05);
		QMap<QString, QVariant>* attriMap = feature->getAttributeMap();
		qDebug() << attriMap->size();
	}
	else if(e->button() == Qt::RightButton){   //右键
       
	}
	else if(e->button() == Qt::MidButton){   //滚轮单击
		//初始化
		screenPointBegin = e->pos();
		screenPointDuring = e->pos();
		screenPointEnd = e->pos();
		originWorldRect = worldRect;
	}
}

void QtFunctionWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(e->buttons() & Qt::LeftButton){ //鼠标移动并按住左键
       
	}else if(e->buttons() & Qt::RightButton){   //鼠标移动并按住右键
	
	}else if(e->buttons() & Qt::MidButton){   //鼠标移动并按住滚轮 ----  地图移动
		//更新中间点坐标
		screenPointDuring = e->pos();
		refreshWorldRectForTrans(screenPointBegin, screenPointDuring);
		project();
		update();
	}
}

void QtFunctionWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {   //右键

	}
	else if (e->button() == Qt::RightButton) {   //左键

	}
	else if (e->button() == Qt::MidButton) {   //滚轮单击
		//更新结束点坐标
		screenPointEnd = e->pos();
		refreshWorldRectForTrans(screenPointBegin, screenPointEnd);
		project();
		update();
	}
}

void QtFunctionWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() > 0){   //远离我，向前滚动----地图缩小
		refreshWorldRectForScale(e->pos(), 1.0 / scale);
		project();
		update();
	}else if(e->delta() < 0) {      //靠近我，向后滚动----地图放大
		refreshWorldRectForScale(e->pos(), scale);
		project();
		update();
	}
}

void QtFunctionWidget::releaseVaos(GeoLayer* layer)
{
	if (isExist(layer)) {
		QList<QOpenGLBuffer*>* list = layerBosMap[layer];
		for (int j = 0; j < list->size(); j++) {
			list->at(j)->destroy();
			delete list->at(j);
		}
		for (int i = 0; i < layer->size(); i++) {
			GeoFeature* feature = layer->getFeatureAt(i);
			QList<QOpenGLVertexArrayObject*>* list = featureVaosMap[layer->getFeatureAt(i)];
			for (int j = 0; j < list->size(); j++) {
				delete list->at(j);
			}
			delete list;
			featureVaosMap.remove(feature);
		}
	}
}

void QtFunctionWidget::deleteLayer(GeoLayer * layer)
{
	if (isExist(layer)) {
		releaseVaos(layer);
		delete map->remove(layer);
		if (!map->size()) {
			hasSetRect = false;
			hasWaH = false;
		}
		else {
			switchLayer(map->getLayerAt(0));
		}
	}
}

//平移
void QtFunctionWidget::refreshWorldRectForTrans(QPoint begin, QPoint end)
{
	QPointF worldPointBegin = screenToWorld(begin);
	QPointF worldPointEnd = screenToWorld(end);
	QPointF moveVector = worldPointEnd - worldPointBegin;
	QPointF leftTopPoint = originWorldRect.topLeft();
	QPointF rightBottomPoint = originWorldRect.bottomRight();
	worldRect = QRectF(leftTopPoint - moveVector, rightBottomPoint - moveVector);
}

//缩放
void QtFunctionWidget::refreshWorldRectForScale(QPoint originScreen, float scale)
{
	QPointF originWorld = screenToWorld(originScreen);
	QPointF leftTop = worldRect.topLeft();
	QPointF rightBottom = worldRect.bottomRight();
	QPointF newLeftTop = (leftTop - originWorld)*scale + originWorld;
	QPointF newRightBottom = (rightBottom - originWorld)*scale + originWorld;
	worldRect = QRectF(newLeftTop, newRightBottom);
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

	//修正横纵比
	if (abs(layerRadio) > abs(viewRadio)) {
		layerHeight = -abs(layerWidth / viewRadio);
	}
	else {
		layerWidth = abs(layerHeight*viewRadio);
	}
	QPoint leftTop(center.x() - layerWidth * 1.0 / 2, center.y() - layerHeight * 1.0 / 2);
	worldRect = QRect(leftTop, QSize(layerWidth, layerHeight));
}

//TODO 判断resize的拉伸方向
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

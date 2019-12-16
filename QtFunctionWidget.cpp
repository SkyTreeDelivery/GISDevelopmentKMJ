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
	worldRect = QRectF(QPointF(center.x() - width / 2, center.y() - height / 2)
		, QPointF(center.x() + width / 2, center.y() + height / 2));
	this->w = w;
	this->h = h;
    project();    //����ͶӰ
}



void QtFunctionWidget::paintGL(){
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	loadWaitLayers();   //���еĹ���gl������߼��ڴ�ִ��
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
				Render* render = layer->getRender();
				//�����߿�
				if (type == EnumType::POINT) { glLineWidth(render->getMarkerSymbol()->getOutline()->getWidth()); }
				else if (type == EnumType::POLYLINE) { glLineWidth(render->getLineSymbol()->getWidth()); }
				else if (type = EnumType::POLYGON) { glLineWidth(render->getFillSymbol()->getOutline()->getWidth()); }
				if (layer->size() && featureVaosMap.size()) {
					int renderLayerNum = featureVaosMap[layer->getFeatureAt(0)]->size();
					for (int i = 0; i < renderLayerNum; i++) {
						bool renderColorChanged = true;
						for (int m = 0; m < layer->size(); m++) {
							GeoFeature* feature = layer->getFeatureAt(m);
							QOpenGLVertexArrayObject* vao = featureVaosMap[feature]->at(i);
							QOpenGLVertexArrayObject::Binder vaoBind(vao);
							int renderType = vao->property("renderType").toInt();
							if (renderColorChanged) {  //������Ⱦ����ɫ,��������Ʋ�֧�ֲַ���ɫ����Ҫ֧�֣���Ҫÿ��feature����vao����һ����ɫ
								setDefaultRenderColor(render,renderType);
								renderColorChanged = false;
							}
							if (layer->getSelectedFeatures().size()) {  //���ñ�ѡ�����ɫ����
								if (layer->hasSelected(feature)) {
									setSelectedRenderColor(render, renderType);
									renderColorChanged = true;
								}
							}
							//�ж�buffer���ͣ����ƣ�note��Ĭ����Ҫebo��ֻ�����ʷ���䣬��Ϊ���������Բ�����ebo��Ч��û�����Բ�𣬶��ʷ����ʹ��ebo������Ч��
							if (vao->property("bufferType").toInt() == EnumType::bufferType::VBO) {
								switch (vao->property("renderType").toInt())
								{
								case EnumType::renderType::MARKER_FILL:
									glDrawArrays(GL_POINT, 0, layer->getFeatureAt(m)->getGeometry()->size());
									break;
								case EnumType::renderType::MARKER_LINE:
									glDrawArrays(GL_LINE_LOOP, 0, layer->getFeatureAt(m)->getGeometry()->size());
									break;
								case EnumType::renderType::LINE_LINE:
									glDrawArrays(GL_LINE_LOOP, 0, layer->getFeatureAt(m)->getGeometry()->size());
									break;
								case EnumType::renderType::FILL_LINE:
									glDrawArrays(GL_LINE_LOOP, 0, layer->getFeatureAt(m)->getGeometry()->size());
									break;
								default:
									break;
								}
							}
							else if (vao->property("bufferType").toInt() == EnumType::bufferType::EBO) {
								switch (vao->property("renderType").toInt())
								{
								case EnumType::renderType::FILL_FILL:
									glDrawElements(GL_TRIANGLES, vao->property("indexNum").toInt(), GL_UNSIGNED_INT, 0);
									break;
								default:
									break;
								}
							}
						}
					}
				}
			}
		}
    }
	shaderProgram.release();
}



//�������⿪�ţ�ֻ������ȴ���������Ӵ�����ͼ�㣬�Ӷ�ʵ���첽����
void QtFunctionWidget::addlayer(GeoLayer* layer)
{
	if (!isExist(layer)) {
		waitLoadedLayers.push_back(layer);
	}
}
void QtFunctionWidget::renderLayer(GeoLayer * layer)
{
	if (layer) {
		waitLoadedLayers.push_back(layer);
	}
}

void QtFunctionWidget::switchOpenrateMode(int operateMode)
{
	this->operateMode = operateMode;
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

//note that ����ҪʹinitLayer�������Ĵ����б���ʾ���ã������ͼ����в�������ʹ��addlayer��deletelayer ����������ԭ������
//��gl��ͼ��صĺ���������init��resize��paintgl�б����ã�����֮�ⲻ����أ������Ҫ�����ݼ��غͻ�ͼ���룬���첽�ķ�ʽ���������������
//�������֪��������������֮����ûط���ʲô�����Լ����ԣ�������debug�˺ܾòȵĿӡ���ʹ��bindVaos������ʱ������һ�����������Ƕ����ڴ��д��������������ƻ�������ԭ��δ֪
void QtFunctionWidget::initLayer(GeoLayer* layer)
{
	if(!isExist(layer)){  //��һ�μ���
		map->addLayer(layer);
		currentLayer = layer;
		bindVaos(layer);
	}
	else {  //��������
		currentLayer = layer;
		if (layer->getDataChangedType() != EnumType::dataChangedType::NOCHANGEDATA) {
			switch (layer->getDataChangedType())
			{
			case EnumType::dataChangedType::SPATICALDATA:
				releaseVaos(layer);
				bindVaos(layer);
				layer->setDataChangedType(EnumType::dataChangedType::NOCHANGEDATA);
				project();    //�ռ����ݷ����˱仯����˵���һ�Σ����ﲻ����update()
				break;
			case EnumType::dataChangedType::COLORDATA:  //��Ϊ���������в�����ɫ�����ݣ�ĿǰɶҲ����
				layer->setDataChangedType(EnumType::dataChangedType::NOCHANGEDATA);
				break;
			default:
				break;
			}
		}
	}
	if(!hasSetRect){   //��һ������
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

//������ش�����ͼ�㣬����paintGL�б�����
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
	for(int i = 0;i < currentLayer->size();i++){
		GeoFeature* feature = layer->getFeatureAt(i);
		GeoGeometry* geometry = feature->getGeometry();
		//���ٿռ䲢�����¼
		int size = 3;
		if (geometry->getType() == EnumType::POINT) {
			GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			GeoPoint* point = (GeoPoint*)geometry;
			float x = point->getXf();
			float y = point->getYf();
			vertices[0] = GLfloat(x);
			vertices[1] = GLfloat(y);
			vertices[2] = GLfloat(0.0);

			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			QOpenGLVertexArrayObject* vao = new QOpenGLVertexArrayObject();
			vaoList->push_back(vao);
			vao->setProperty("bufferType", QVariant(EnumType::bufferType::VBO));
			vao->setProperty("geoType", QVariant(EnumType::geometryType::POINT));
			vao->setProperty("renderType", QVariant(EnumType::renderType::MARKER_FILL));
			featureVaosMap.insert(feature, vaoList);
			
			QOpenGLVertexArrayObject::Binder vaoBind0(vao);
			QOpenGLBuffer* vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			vbo->create();
			vbo->bind();
			vbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);
			
			boList->push_back(vbo);

			int posAttr = -1;
			//ͨ���������Ӷ�����ɫ���л�ȡ����Ӧ������location
			posAttr = shaderProgram.attributeLocation("aPos");
			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.release();
			vbo->release();
		}
		else if (geometry->getType() == EnumType::POLYLINE) {
			GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			GeoPolyline* polyline = (GeoPolyline*)geometry;
			for (int j = 0; j < geometry->size(); j++) {
				GeoPoint* point = polyline->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j * size + 0] = GLfloat(x);
				vertices[j * size + 1] = GLfloat(y);
				vertices[j * size + 2] = GLfloat(0.0);
			}

			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			QOpenGLVertexArrayObject* vao = new QOpenGLVertexArrayObject();
			vaoList->push_back(vao);
			vao->setProperty("bufferType", QVariant(EnumType::bufferType::VBO));
			vao->setProperty("geoType", QVariant(EnumType::geometryType::POLYLINE));
			vao->setProperty("renderType", QVariant(EnumType::renderType::LINE_LINE));
			featureVaosMap.insert(feature, vaoList);

			QOpenGLVertexArrayObject::Binder vaoBind0(vao);
			QOpenGLBuffer* vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			vbo->create();
			vbo->bind();
			vbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);

			boList->push_back(vbo);

			int posAttr = -1;
			//ͨ���������Ӷ�����ɫ���л�ȡ����Ӧ������location
			posAttr = shaderProgram.attributeLocation("aPos");
			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.release();
			vbo->release();
		}
		else if (geometry->getType() == EnumType::POLYGON) {
			//���ò���
			QList<QOpenGLVertexArrayObject*>* vaoList = new QList<QOpenGLVertexArrayObject*>();
			featureVaosMap.insert(feature, vaoList);
			GeoPolygon* polygon = (GeoPolygon*)geometry;
			GLfloat* vertices = NULL;

			//�����ʷ����� - level0
			gpc_tristrip tristrip;
			util::tesselation(polygon, &tristrip); 

			int stripNum = tristrip.num_strips;		//���Ƿ�����
			int triNum = 0;		//���Ǹ���
			int vertexNum = 0;		//���������û���ظ�����
			for (int i = 0; i < stripNum; i++)
			{
				triNum += tristrip.strip[i].num_vertices - 2;//����һ���ж���������
				vertexNum += tristrip.strip[i].num_vertices;//����һ���ж��ٶ���
			}

			//������������
			vertices = new GLfloat[size * vertexNum];
			int idx = 0;
			for (int i = 0; i < stripNum; i++) {  //iΪÿ����������
				gpc_vertex_list list = tristrip.strip[i];
				for (int j = 0; j < list.num_vertices; j++) {  //jΪ���������е�ÿ������
					gpc_vertex vertex = list.vertex[j];
					float x = vertex.x;
					float y = vertex.y;
					vertices[idx * size + 0] = GLfloat(x);
					vertices[idx * size + 1] = GLfloat(y);
					vertices[idx * size + 2] = GLfloat(0.0);
					idx++;
				}
			}

			//������������
			unsigned int *indices = new unsigned int[triNum * 3];
			int idx1 = 0;
			int cumulatedVertex = 0;
			for (int i = 0; i < stripNum; i++) {//iΪÿ����������
				gpc_vertex_list list = tristrip.strip[i];
				for (int j = 0; j < list.num_vertices - 2; j++) {//jΪ���������е�ÿ��������
					for (int k = 0; k < 3; k++) {//kΪ0-3,����ÿ���������������������
						indices[idx1++] = cumulatedVertex + j + k ;
					}
				}
				cumulatedVertex += list.num_vertices;
			}

			QOpenGLVertexArrayObject* fillVao = new QOpenGLVertexArrayObject();
			vaoList->push_back(fillVao);
			fillVao->setProperty("bufferType", QVariant(EnumType::bufferType::EBO));
			fillVao->setProperty("indexNum",triNum * 3);
			fillVao->setProperty("geoType", QVariant(EnumType::geometryType::POLYGON));
			fillVao->setProperty("renderType", QVariant(EnumType::renderType::FILL_FILL));
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
			//ͨ���������Ӷ�����ɫ���л�ȡ����Ӧ������location
			posAttr = shaderProgram.attributeLocation("aPos");
			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.enableAttributeArray(posAttr);
			shaderProgram.release();
			fillVbo->release();

			/*************************************************************************/

			//�߽����� - level1

			vertices = (GLfloat*)malloc(sizeof(GLfloat)*(geometry->size()) * size);
			for (int j = 0; j < geometry->size(); j++) {
				GeoPoint* point = polygon->getPointAt(j);
				float x = point->getXf();
				float y = point->getYf();
				vertices[j * size + 0] = GLfloat(x);
				vertices[j * size + 1] = GLfloat(y);
				vertices[j * size + 2] = GLfloat(0.0);
			}

			QOpenGLVertexArrayObject* outlineVao = new QOpenGLVertexArrayObject();
			outlineVao->setProperty("bufferType", QVariant(EnumType::bufferType::VBO));
			outlineVao->setProperty("geoType", QVariant(EnumType::geometryType::POLYGON));
			outlineVao->setProperty("renderType", QVariant(EnumType::renderType::FILL_LINE));
			vaoList->push_back(outlineVao);

			QOpenGLVertexArrayObject::Binder vaoBind1(outlineVao);
			QOpenGLBuffer* outlineVbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			outlineVbo->create();
			outlineVbo->bind();
			outlineVbo->allocate(vertices, sizeof(GLfloat)*(geometry->size())*size);

			posAttr = -1;
			//ͨ���������Ӷ�����ɫ���л�ȡ����Ӧ������location
			posAttr = shaderProgram.attributeLocation("aPos");
			shaderProgram.setAttributeBuffer(posAttr, GL_FLOAT, sizeof(GLfloat) * 0, 3, sizeof(GLfloat) * size);
			shaderProgram.enableAttributeArray(posAttr);
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


void QtFunctionWidget::on_addLayerData(GeoLayer* layer) {
	addlayer(layer);
	update();
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
	currentLayer->setDataChangedType(EnumType::dataChangedType::COLORDATA);
	renderLayer(currentLayer);   //��ʽ�����˱仯���������Ⱦ
}


void QtFunctionWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton){   //�Ҽ�
		if (operateMode == EnumType::operateMode::IDENTIFY) {
			QPointF worldPoint = screenToWorld(e->pos());
			GeoFeature* feature = currentLayer->Identify(&GeoPoint(worldPoint), currentLayer, worldRect.width() / 250);
			currentLayer->setSelectMode(EnumType::selectMode::SINGLEMODE);
			currentLayer->selectFeature(feature);
		}
	}else if(e->button() == Qt::RightButton){   //���
		if (operateMode == EnumType::operateMode::IDENTIFY) {
			operateMode = EnumType::operateMode::NORMOL;
	   }
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

void QtFunctionWidget::setDefaultRenderColor(Render* render, int type)
{
	switch (type)
	{
	case EnumType::renderType::MARKER_FILL:
		shaderProgram.setUniformValue("color", render->getMarkerSymbol()->getColor()); //note:�����setUniformValue���Զ���qcolor�е�0-255��ɫ������һ��
		break;
	case EnumType::renderType::MARKER_LINE:
		shaderProgram.setUniformValue("color", render->getMarkerSymbol()->getOutline()->getColor());
		break;
	case EnumType::renderType::LINE_LINE:
		shaderProgram.setUniformValue("color", render->getLineSymbol()->getColor());
		break;
	case EnumType::renderType::FILL_FILL:
		shaderProgram.setUniformValue("color", render->getFillSymbol()->getColor());
		break;
	case EnumType::renderType::FILL_LINE:
		shaderProgram.setUniformValue("color", render->getFillSymbol()->getOutline()->getColor());
		break;
	default:
		break;
	}
}

void QtFunctionWidget::setSelectedRenderColor(Render * render, int type)
{
	switch (type)
	{
	case EnumType::renderType::MARKER_FILL:
		shaderProgram.setUniformValue("color", render->getSelectionMarkerSymbol()->getColor()); //note:�����setUniformValue���Զ���qcolor�е�0-255��ɫ������һ��
		break;
	case EnumType::renderType::MARKER_LINE:
		shaderProgram.setUniformValue("color", render->getSelectionMarkerSymbol()->getOutline()->getColor());
		break;
	case EnumType::renderType::LINE_LINE:
		shaderProgram.setUniformValue("color", render->getSelectionLineSymbol()->getColor());
		break;
	case EnumType::renderType::FILL_FILL:
		shaderProgram.setUniformValue("color", render->getSelectionFillSymbol()->getColor());
		break;
	case EnumType::renderType::FILL_LINE:
		shaderProgram.setUniformValue("color", render->getSelectionFillSymbol()->getOutline()->getColor());
		break;
	default:
		break;
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

//ƽ��
void QtFunctionWidget::refreshWorldRectForTrans(QPoint begin, QPoint end)
{
	QPointF worldPointBegin = screenToWorld(begin);
	QPointF worldPointEnd = screenToWorld(end);
	QPointF moveVector = worldPointEnd - worldPointBegin;
	QPointF leftTopPoint = originWorldRect.topLeft();
	QPointF rightBottomPoint = originWorldRect.bottomRight();
	worldRect = QRectF(leftTopPoint - moveVector, rightBottomPoint - moveVector);
}

//����
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

//TODO �ж�resize�����췽��
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
	return EnumType::NOCHANGESIZE;
}

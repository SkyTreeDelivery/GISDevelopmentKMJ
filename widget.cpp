#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include <QPushButton>
#include <QMenuBar>
#include <QAction>
#include "util.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include "GeoPoint.h"
#include <QVBoxLayout>
#include "QtFunctionWidget.h"
#include <qlabel.h>
#include <qtreewidget.h>
#include <qplaintextedit.h>
#include <qt_windows.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

	//ʹ��new����Ϊ���������������ͻᱻ����
	//�����ַ�ʽ���layout��widget��һ�����ڶ���widget��layout����ʱ��ӣ����ʿ��Կ�����һ����״�ṹ����ӣ��������ϣ�
	//һ�����ö���wedget��layoutȥset���Զ����£�
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	this->setLayout(mainLayout);   //������ö����widgetȥsetlayout
	mainLayout->addWidget(genMenuBar());
	QHBoxLayout* showLayout = new QHBoxLayout();
	mainLayout->addLayout(showLayout);
	QVBoxLayout* fileAOutLayout = new QVBoxLayout();
	showLayout->addLayout(fileAOutLayout,2);
	glw = new QtFunctionWidget();
	showLayout->addWidget(glw,5);

	//fileAOutLayout add
	QLabel* filelLable = new QLabel("layer management");
	fileAOutLayout->addWidget(filelLable,1);
	fileTree = new QTreeWidget();
	fileAOutLayout->addWidget(fileTree,10);
	QLabel* outlLable = new QLabel("out infomation");
	fileAOutLayout->addWidget(outlLable,1);
	outTextEdit = new QPlainTextEdit();
	fileAOutLayout->addWidget(outTextEdit,10);
	//��δ���˵����layout���widget��layout����addWidget��addLayout����widget����addwidget��������ͨ��setLayout���layout
	//������Ϊһ��layout�ǿ�����Ӷ��layout��wedget�ģ���һ��wedget�������wedget��֮���������һ��layout
	//add����������set����һ��
	/*QVBoxLayout* testLayout = new QVBoxLayout();
	QLabel* testLable = new QLabel("nihao");
	testLayout->addWidget(testLable);
	outTextEdit->setLayout(testLayout);*/

	fileTree->setColumnCount(3);
	fileTree->setHeaderLabels(QStringList()<<"name"<<"class"<<"num");
	fileTree->setColumnWidth(0,100);
	fileTree->setColumnWidth(1,100);
	fileTree->setColumnWidth(2,100);

	//fileTree->setSortingEnabled(true);
//	fileTree->header()->setSortIndicatorShown(false);

	//��������Ҽ�
	//ͬʱֻ����ʾһ���˵���ÿ���˵�����һ��������ʱ�ᱻ����
	//��Щ���Ĭ�Ͼ���һ���˵�������Щ���û�в˵�����Ҫ�Լ�����
	fileTree->setContextMenuPolicy(Qt::CustomContextMenu);
	//glw->setContextMenuPolicy(Qt::CustomContextMenu);

	//�Ƽ����������ַ�ʽдconnect�����������Ƿ�����Ϊ����������򲻴��ڵ��µ����⣬�����ٱ���׶��ҵ��������������к���Ч��ȴ�޷���飡������
	//��صĲ����ᱻ�Զ����ݣ�ͬʱ��ʡ�˲����Ĵ���
	connect(fileTree,&QOpenGLWidget::customContextMenuRequested,this,&Widget::on_fileTree_contextMenu_request);
	//connect(glw,&QOpenGLWidget::customContextMenuRequested,this,&Widget::on_glw_contextMenu_request);

	connect(this, &Widget::deleteLayerSignal,glw,&QtFunctionWidget::on_deleteLayerData);
	connect(this, &Widget::zoomToLayerSignal,glw,&QtFunctionWidget::on_zoomToLayerRect);

	initRightClickMenu();
}

Widget::~Widget()
{
    delete ui;
}

QMenuBar* Widget::genMenuBar(){
    // QMenuBar
    QMenuBar* menuBar = new QMenuBar();

    //QMenu
    QMenu *pMenu = new QMenu("import file",this);

    //import file QMenu
    QAction* import_geojson = pMenu->addAction(QString::fromLocal8Bit("import geojson"));
    pMenu->addSeparator();
    QAction* import_shapefile = pMenu->addAction(QString::fromLocal8Bit("import shapefile"));

    //event
    connect(import_geojson,SIGNAL(triggered()),this,SLOT(on_importGeoJson_action_triggered()));
    connect(import_shapefile,SIGNAL(triggered()),this,SLOT(on_importShapeFile_action_triggered()));

    //addMenu
    menuBar->addMenu(pMenu);
	return menuBar;
}

void Widget::initRightClickMenu()
{
    fileMenu = new QMenu(this);
	QAction* zoomToLayerAction = fileMenu->addAction(QString("zoom to layer"));
	QAction* deleteLayerAction = fileMenu->addAction(QString("delete Layer"));
	
	connect(zoomToLayerAction, SIGNAL(triggered()), this, SLOT(on_zoomToLayer_action_triggered()));
	connect(deleteLayerAction, SIGNAL(triggered()), this, SLOT(on_deleteLayer_action_triggered()));
	/*glwMenu = new QMenu(this);
	QAction* deleteLayerActionTest = glwMenu->addAction(QString("glw"));*/
	//connect(deleteLayerActionTest, SIGNAL(triggered()), this, SLOT(on_deleteLayer_action_triggered()));
}

//����geojson�ļ�
void Widget::on_importGeoJson_action_triggered()
{
    QString curPath = QDir::currentPath();
    QString dlgTitle = "open a geojsonFile";
    geojson_filename = QFileDialog::getOpenFileName(this,dlgTitle,curPath);
    qDebug() << geojson_filename;
	if(geojson_filename != ""){
		GeoLayer* layer = util::openGeoJsonByCJson(geojson_filename);
		glw->addlayer(layer);
		addLayerInfo(layer);
	}
}

//ʹ��gdal����shapefile�ļ�
void Widget::on_importShapeFile_action_triggered()
{
    QString curPath = QDir::currentPath();
    QString dlgTitle = "open a shapeFile";
    shapefile_filename = QFileDialog::getOpenFileName(this,dlgTitle,curPath);
    qDebug() << shapefile_filename;
}

void Widget::on_fileTree_contextMenu_request(const QPoint &pos)
{
	//����filetree�Ҽ��¼�
	curItem = fileTree->currentItem();
	if(curItem == NULL){
		return;
	}
	fileMenu->exec(QCursor::pos());
}

void Widget::on_glw_contextMenu_request(const QPoint &pos)
{
	//����filetree�Ҽ��¼�
	glwMenu->exec(QCursor::pos());
}

void Widget::on_deleteLayer_action_triggered()
{
	QVariant fullpathV = curItem->data(0,Qt::DisplayRole);
	QString fullpath = fullpathV.toString();
	emit deleteLayerSignal(fullpath);
	fileTree->takeTopLevelItem(fileTree->currentIndex().row());
}


void Widget::on_zoomToLayer_action_triggered()
{
	QVariant fullpathV = curItem->data(0,Qt::DisplayRole);
	QString fullpath = fullpathV.toString();
	emit zoomToLayerSignal(fullpath);
}


void Widget::addLayerInfo(GeoLayer* layer)
{
	QString name = layer->getFullPath();
	QString type = layer->getTypeString();
	QString size = QVariant(layer->size()).toString();
	QTreeWidgetItem* A = new QTreeWidgetItem(QStringList()<< name << type << size);
	A->setCheckState(0, Qt::Checked);
	fileTree->addTopLevelItem(A);
}
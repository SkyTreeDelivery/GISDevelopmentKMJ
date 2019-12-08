#include "mainwindow.h"
#include "ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), currItem(NULL), sw(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	initMenuBar();
	//使用new是因为过了这个函数对象就会被销毁
	//有两种方式添加layout或widget，一种是在顶层widget或layout构造时添加，本质可以看做是一个树状结构的添加（从下向上）
	//一种是用顶层wedget或layout去set（自顶向下）
	QWidget* cenWidget = new QWidget();
	this->setCentralWidget(cenWidget);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	cenWidget->setLayout(mainLayout);   //这就是用顶层的widget去setlayout

	QHBoxLayout* showLayout = new QHBoxLayout();
	mainLayout->addLayout(showLayout);
	QVBoxLayout* fileAOutLayout = new QVBoxLayout();
	showLayout->addLayout(fileAOutLayout, 2);
	glw = new QtFunctionWidget();
	showLayout->addWidget(glw, 5);

	//fileAOutLayout add
	QLabel* filelLable = new QLabel("layer management");
	fileAOutLayout->addWidget(filelLable, 1);
	fileTree = new QTreeWidget();
	fileAOutLayout->addWidget(fileTree, 10);
	QLabel* outlLable = new QLabel("out infomation");
	fileAOutLayout->addWidget(outlLable, 1);
	outTextEdit = new QPlainTextEdit();
	fileAOutLayout->addWidget(outTextEdit, 10);
	//这段代码说明，layout添加widget与layout是用addWidget与addLayout，而widget不能addwidget，但可以通过setLayout添加layout
	//这是因为一个layout是可以添加多个layout和wedget的，而一个wedget不能添加wedget，之恩给你添加一个layout
	//add代表多个，而set代表一个
	/*QVBoxLayout* testLayout = new QVBoxLayout();
	QLabel* testLable = new QLabel("nihao");
	testLayout->addWidget(testLable);
	outTextEdit->setLayout(testLayout);*/

	fileTree->setColumnCount(3);
	fileTree->setHeaderLabels(QStringList() << "name" << "class" << "num");
	fileTree->setColumnWidth(0, 100);
	fileTree->setColumnWidth(1, 100);
	fileTree->setColumnWidth(2, 100);

	//fileTree->setSortingEnabled(true);
//	fileTree->header()->setSortIndicatorShown(false);

	//处理鼠标右键
	//同时只能显示一个菜单，每个菜单在另一个被调用时会被销毁
	//有些组件默认就有一个菜单，而有些组件没有菜单，需要自己设置
	fileTree->setContextMenuPolicy(Qt::CustomContextMenu);
	//glw->setContextMenuPolicy(Qt::CustomContextMenu);

	//推荐以下面这种方式写connect函数，作用是防治因为函数名出错或不存在导致的问题，可以再编译阶段找到，而不至于运行毫不效果却无法检查！！！！
	//相关的参数会被自动传递，同时节省了参数的传递
	connect(fileTree, &QOpenGLWidget::customContextMenuRequested, this, &MainWindow::on_fileTree_contextMenu_request);
	//connect(glw,&QOpenGLWidget::customContextMenuRequested,this,&Widget::on_glw_contextMenu_request);

	connect(this, &MainWindow::addLayerSignal, glw, &QtFunctionWidget::on_addLayerData);
	connect(this, &MainWindow::deleteLayerSignal, glw, &QtFunctionWidget::on_deleteLayerData);
	connect(this, &MainWindow::zoomToLayerSignal, glw, &QtFunctionWidget::on_zoomToLayerRect);
	connect(this, &MainWindow::setSymbolSignal, glw, &QtFunctionWidget::on_setSymbol);

	connect(fileTree, &QTreeWidget::itemChanged, this, &MainWindow::on_item_changed);

	initRightClickMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initRightClickMenu()
{
	fileMenu = new QMenu(this);
	QAction* zoomToLayerAction = fileMenu->addAction(QString("zoom to layer"));
	QAction* deleteLayerAction = fileMenu->addAction(QString("delete Layer"));
	fileMenu->addSeparator();
	QAction* set_SLD = fileMenu->addAction(QString::fromLocal8Bit("read SLD"));
	QAction* set_style = fileMenu->addAction(QString::fromLocal8Bit("set style"));
	connect(zoomToLayerAction, SIGNAL(triggered()), this, SLOT(on_zoomToLayer_action_triggered()));
	connect(deleteLayerAction, SIGNAL(triggered()), this, SLOT(on_deleteLayer_action_triggered()));
	connect(set_SLD, SIGNAL(triggered()), this, SLOT(on_setSLD_action_triggered()));
	connect(set_style, SIGNAL(triggered()), this, SLOT(on_setStyle_action_triggered()));
	/*glwMenu = new QMenu(this);
	QAction* deleteLayerActionTest = glwMenu->addAction(QString("glw"));*/
	//connect(deleteLayerActionTest, SIGNAL(triggered()), this, SLOT(on_deleteLayer_action_triggered()));
}

void MainWindow::initMenuBar()
{
	QMenuBar* menuBar = this->menuBar();
	QMenu *pMenu = new QMenu("import file", this);

	//import file QMenu
	QAction* import_geojson = pMenu->addAction(QString::fromLocal8Bit("import geojson"));
	QAction* import_shapefile = pMenu->addAction(QString::fromLocal8Bit("import shapefile"));

	//event
	connect(import_geojson, SIGNAL(triggered()), this, SLOT(on_importGeoJson_action_triggered()));
	connect(import_shapefile, SIGNAL(triggered()), this, SLOT(on_importShapeFile_action_triggered()));

	//addMenu
	menuBar->addMenu(pMenu);
}

//导入geojson文件
void MainWindow::on_importGeoJson_action_triggered()
{
	QString curPath = QDir::currentPath();
	QString dlgTitle = "open a geojsonFile";
	geojson_filename = QFileDialog::getOpenFileName(this, dlgTitle, curPath);
	qDebug() << geojson_filename;
	if (geojson_filename != "") {
		GeoLayer* layer = util::openGeoJson(geojson_filename);
		if (!glw->isExist(geojson_filename)) {
			emit addLayerSignal(layer);
			addLayerInfo(layer);
		}
	}
}

//使用gdal导入shapefile文件
void MainWindow::on_importShapeFile_action_triggered()
{
	QString curPath = QDir::currentPath();
	QString dlgTitle = "open a shapeFile";
	shapefile_filename = QFileDialog::getOpenFileName(this, dlgTitle, curPath);
	qDebug() << shapefile_filename;
}

void MainWindow::on_setSLD_action_triggered()
{
	QString curPath = QDir::currentPath();
	QString dlgTitle = "open a SLD";
	geojson_filename = QFileDialog::getOpenFileName(this, dlgTitle, curPath);
	if (geojson_filename != "") {
		FillSymbol* fillSymbol = util::parseSLD_Fill(geojson_filename);
		emit setSymbolSignal(fillSymbol);
	}
}

void MainWindow::on_setStyle_action_triggered()
{
	QTreeWidgetItem* newItem = fileTree->currentItem();
	initStyleWidget(newItem);
}

void MainWindow::initStyleWidget(QTreeWidgetItem* newItem) {
	if (newItem != currItem) {
		if (currItem != NULL) {
			sw->close();  //必须先关闭再close;
			delete sw;
		}
		GeoLayer* layer = itemLayerMap[newItem];
		sw = new StyleWidget(layer);
		connect(sw, &StyleWidget::renderLayerSignal, this, &MainWindow::on_style_Changed);
		sw->setWindowTitle(QString("set style ::") + layer->getFullPath());
		connect(sw, &StyleWidget::closeSignal, this, &MainWindow::on_styleWidget_closed);
		sw->show();
		currItem = newItem;
	}
}

void MainWindow::on_fileTree_contextMenu_request(const QPoint &pos)
{
	//处理filetree右键事件
	curItem = fileTree->currentItem();
	if (curItem == NULL) {
		return;
	}
	fileMenu->exec(QCursor::pos());
}

void MainWindow::on_glw_contextMenu_request(const QPoint &pos)
{
	//处理filetree右键事件
	glwMenu->exec(QCursor::pos());
}

void MainWindow::on_item_changed(QTreeWidgetItem *item)
{
	GeoLayer* layer = itemLayerMap[item];
	if (item->checkState(0) == Qt::Checked) {
		layer->setVisable(true);
		glw -> update();
	}
	else {
		layer->setVisable(false);
		glw->update();
	}
}

void MainWindow::on_styleWidget_closed()
{
	currItem = NULL;
}

void MainWindow::on_style_Changed(GeoLayer * layer)
{
	glw->renderLayer(layer);
	glw->update();
}

void MainWindow::on_deleteLayer_action_triggered()
{
	QVariant fullpathV = curItem->data(0, Qt::DisplayRole);
	QString fullpath = fullpathV.toString();
	emit deleteLayerSignal(fullpath);
	fileTree->takeTopLevelItem(fileTree->currentIndex().row());
}


void MainWindow::on_zoomToLayer_action_triggered()
{
	QVariant fullpathV = curItem->data(0, Qt::DisplayRole);
	QString fullpath = fullpathV.toString();
	emit zoomToLayerSignal(fullpath);
}


void MainWindow::addLayerInfo(GeoLayer* layer)
{
	QString name = layer->getFullPath();
	QString type = layer->getTypeString();
	QString size = QVariant(layer->size()).toString();
	QTreeWidgetItem* A = new QTreeWidgetItem(QStringList() << name << type << size);
	itemLayerMap.insert(A, layer);
	A->setCheckState(0, Qt::Checked);
	fileTree->addTopLevelItem(A);
}

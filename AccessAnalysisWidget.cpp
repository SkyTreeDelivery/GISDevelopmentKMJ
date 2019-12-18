#include "AccessAnalysisWidget.h"
#include "ui_AccessAnalysisWidget.h"
#include "GeoMap.h"
AccessAnalysisWidget::AccessAnalysisWidget(GeoMap* map, QWidget *parent)
	: QWidget(parent),map(map)
{
	ui = new Ui::AccessAnalysisWidget();
	ui->setupUi(this);
	initWidget();
	connect(ui->okButton, &QPushButton::clicked, this, &AccessAnalysisWidget::on_runTool);
	connect(ui->cancelButton, &QPushButton::clicked, this, &AccessAnalysisWidget::close);
}

AccessAnalysisWidget::~AccessAnalysisWidget()
{
	delete ui;
}

void AccessAnalysisWidget::initWidget()
{
	ui->startBox->addItem("将当前图层所选点要素作为查询起点");
	ui->startBox->clear();
	ui->arcBox->clear();
	ui->nodeBox->clear();
	ui->timeLimitTextLine->setText("");
	for (int i = 0; i < map->size(); i++) {
		GeoLayer* layer = map->getLayerAt(i);
		int type = layer->getType();
		if (type == EnumType::POINT) {
			ui->targetPointLayerBox->addItem(layer->getName(), QVariant(layer->getFullPath()));
			ui->nodeBox->addItem(layer->getName(), QVariant(layer->getFullPath()));
		}
		else if (type == EnumType::POLYLINE) {
			ui->arcBox->addItem(layer->getName(), QVariant(layer->getFullPath()));
		}
	}
}

void AccessAnalysisWidget::connectDatabase(QString databaseUrl)
{
	PostgisHelper helper(databaseUrl);
	if (!dataset) {
		dataset->Release();
		delete dataset;
	}
	dataset = helper.getDataSet();
}

void AccessAnalysisWidget::on_runTool()
{

}

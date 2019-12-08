#include "StyleWidget.h"
#include "ui_widget.h"
#include <qtreewidget.h>
#include <QVBoxLayout>
#include "GeoLayer.h"
#include "EnumType.h"
#include <qheaderview.h>
#include <qcolordialog.h>

StyleWidget::StyleWidget(GeoLayer* layer, QWidget *parent) :
	QWidget(parent),layer(layer),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
	QVBoxLayout* layout = new QVBoxLayout();
	styleTree = new QTreeWidget();
	layout->addWidget(styleTree);
	setLayout(layout);
	resize(500, 200);
	initTree(layer);
	connect(styleTree, &QTreeWidget::itemDoubleClicked, this, &StyleWidget::on_item_clicked);
}

StyleWidget::~StyleWidget()
{
    delete ui;
}

void StyleWidget::closeEvent(QCloseEvent * event)
{
	emit closeSignal();
}

void StyleWidget::initTree(GeoLayer* layer)
{
	styleTree->header()->hide();
	int type = layer->getType();
	if (type == EnumType::POINT) {

	}
	else if (type == EnumType::POLYLINE) {

	}
	else if (type == EnumType::POLYGON) {
		fill = layer->getRender()->getFillSymbol();
		outline = fill->getOutline();
		QTreeWidgetItem* outline = new QTreeWidgetItem(QStringList() << "outline");
		QTreeWidgetItem* fill = new QTreeWidgetItem(QStringList() << "fill");
		styleTree->addTopLevelItem(outline);
		styleTree->addTopLevelItem(fill);
		QTreeWidgetItem* outline_color = new QTreeWidgetItem(QStringList() << "color");
		QTreeWidgetItem* outline_width = new QTreeWidgetItem(QStringList() << "width");
		itemType.insert(outline_color, EnumType::LINECOLOR);
		itemType.insert(outline_width, EnumType::LINEWIDTH);
		outline->addChild(outline_color);
		outline->addChild(outline_width);
		QTreeWidgetItem* fill_color = new QTreeWidgetItem(QStringList() << "color");
		itemType.insert(fill_color, EnumType::FILLCOLOR);
		fill->addChild(fill_color);
		styleTree->expandAll();  //必须在之后调用
	}
}

void StyleWidget::on_item_clicked(QTreeWidgetItem* item) {
	int type = itemType[item];
	if (type == EnumType::LINECOLOR ) {
		QColor result = QColorDialog::getColor(outline->getColor(), this, "line color");
		if (result.isValid()) {
			outline->setColor(result);
			emit renderLayerSignal(layer);
		}
	}
	else if (type == EnumType::FILLCOLOR) {
		QColor result = QColorDialog::getColor(fill->getColor(), this, "fill color");
		if (result.isValid()) {
			fill->setColor(result);
			emit renderLayerSignal(layer);
		}
	}
	else if (type == EnumType::LINEWIDTH) {

	}
}

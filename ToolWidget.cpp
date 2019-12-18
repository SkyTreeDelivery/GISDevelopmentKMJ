#include "ToolWidget.h"
#include "ui_ToolWidget.h"
#include <QVBoxLayout>
#include <qtreewidget.h>
#include <EnumType.h>
#include <qheaderview.h>
#include "kernalDensity.h"
#include "AccessibilityAnalysisTool.h"
#include "AccessAnalysisWidget.h"

ToolWidget::ToolWidget(GeoMap* map, QWidget *parent)
	: QWidget(parent), toolTree(NULL),map(map), aaw(NULL)
{
	ui = new Ui::ToolWidget();
	ui->setupUi(this);
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);
	toolTree = new QTreeWidget();
	mainLayout->addWidget(toolTree);
	initToolTree();
}

ToolWidget::~ToolWidget()
{
	delete ui;
}

void ToolWidget::initToolTree()
{
	toolTree->header()->hide();
	QTreeWidgetItem* spatialToolBox = new QTreeWidgetItem(QStringList() << QStringLiteral("空间分析") << "(spatial analysis)");
	toolTree->addTopLevelItem(spatialToolBox);

	QTreeWidgetItem* nda = new QTreeWidgetItem(QStringList() << QStringLiteral("核密度分析") << "(Nuclear density analysis)");
	QTreeWidgetItem* aa = new QTreeWidgetItem(QStringList() << QStringLiteral("可达性分析") << "(Accessibility analysis)");
	spatialToolBox->addChild(nda);
	spatialToolBox->addChild(aa);
	nda->setData(0, Qt::WhatsThisRole, QVariant(EnumType::sparitalAnalysisTool::NUCLEAR_DENSITY_ANALYSIS));
	aa->setData(0, Qt::WhatsThisRole, QVariant(EnumType::sparitalAnalysisTool::ACCESSIBILITY_ANALYSIS));
	connect(toolTree, &QTreeWidget::itemDoubleClicked, this, &ToolWidget::on_item_clicked);

	toolTree->expandAll();  //必须在之后调用
}


void ToolWidget::on_item_clicked(QTreeWidgetItem* item)
{
	int toolType = item->data(0, Qt::WhatsThisRole).toInt();
	if (toolType == EnumType::sparitalAnalysisTool::NUCLEAR_DENSITY_ANALYSIS) { //核密度

	}
	else if (toolType == EnumType::sparitalAnalysisTool::ACCESSIBILITY_ANALYSIS) { //可达性
		initAccessibilityAnalysisWidget();
	}
}

void ToolWidget::initAccessibilityAnalysisWidget()
{
	if (!aaw) {
		aaw = new AccessAnalysisWidget(map);
	}
	aaw->initWidget();
	aaw->show();
}




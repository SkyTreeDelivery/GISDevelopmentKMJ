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
	QTreeWidgetItem* spatialToolBox = new QTreeWidgetItem(QStringList() << QStringLiteral("�ռ����") << "(spatial analysis)");
	toolTree->addTopLevelItem(spatialToolBox);

	QTreeWidgetItem* nda = new QTreeWidgetItem(QStringList() << QStringLiteral("���ܶȷ���") << "(Nuclear density analysis)");
	QTreeWidgetItem* aa = new QTreeWidgetItem(QStringList() << QStringLiteral("�ɴ��Է���") << "(Accessibility analysis)");
	spatialToolBox->addChild(nda);
	spatialToolBox->addChild(aa);
	nda->setData(0, Qt::WhatsThisRole, QVariant(EnumType::sparitalAnalysisTool::NUCLEAR_DENSITY_ANALYSIS));
	aa->setData(0, Qt::WhatsThisRole, QVariant(EnumType::sparitalAnalysisTool::ACCESSIBILITY_ANALYSIS));
	connect(toolTree, &QTreeWidget::itemDoubleClicked, this, &ToolWidget::on_item_clicked);

	toolTree->expandAll();  //������֮�����
}


void ToolWidget::on_item_clicked(QTreeWidgetItem* item)
{
	int toolType = item->data(0, Qt::WhatsThisRole).toInt();
	if (toolType == EnumType::sparitalAnalysisTool::NUCLEAR_DENSITY_ANALYSIS) { //���ܶ�

	}
	else if (toolType == EnumType::sparitalAnalysisTool::ACCESSIBILITY_ANALYSIS) { //�ɴ���
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




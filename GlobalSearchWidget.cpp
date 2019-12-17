#include "GlobalSearchWidget.h"
#include "ui_GlobalSearchWidget.h"
#include <NLPIR.h>

GlobalSearchWidget::GlobalSearchWidget(GeoMap* map, QWidget *parent)
	: QWidget(parent), map(map)
{
	ui = new Ui::GlobalSearchWidget();
	ui->setupUi(this);
	connect(ui->searchButton, &QPushButton::clicked, this, &GlobalSearchWidget::on_searchButton_clicked); 
	if (!NLPIR_Init(0,1)) {
		qWarning() << QStringLiteral("NLPIR初始化失败");
	}
}

GlobalSearchWidget::~GlobalSearchWidget()
{
	if (NLPIR_Exit()) {
		qWarning() << QStringLiteral("分词系统退出失败");
	}
	delete ui;
}

void GlobalSearchWidget::on_searchButton_clicked()
{
	QString quary = ui->searchInputTextEdit->text();
	if (quary != "") {
		const char* result = NLPIR_ParagraphProcess(quary.toStdString().c_str());
		QString pardsStr = QString::fromLocal8Bit(result);
		QStringList partList = pardsStr.split(" ");
		for (int i = 0; i < partList.size(); i++) {
			QString part = partList.at(i);
			QString word = part.mid(0, part.indexOf("/"));
			QString partOfSpeech = part.mid(part.indexOf("/") + 1);
			for (int j = 0; j < map->size(); j++) {
				GeoLayer* layer = map->getLayerAt(j);
				QString attriName = layer->getGlobalDefaultColomn();
				QList<GeoFeature*> features = layer->search(attriName, word);
				for (int i = 0; i < features.size(); i++) {
					GeoFeature* feature = features.at(i);
					QMap<QString, QVariant>* map = feature->getAttributeMap();
					ui->resultList->addItem((*map)[attriName].toString());
					rowFeatureMap.insert(i, feature);
				}
			}
		}
	}
}

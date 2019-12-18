#include "SearchWidget.h"
#include "ui_SearchWidget.h"
#include "qdebug.h"

SearchWidget::SearchWidget(GeoLayer* layer, QWidget *parent)
	: QWidget(parent),layer(layer),ui(new Ui::SearchWidget)
{
	ui->setupUi(this);
	QList<QString> layerNames = layer->getAttributeNames();
	for (int i = 0; i < layerNames.size(); i++) {
		ui->attriNameComboBox->addItem(layerNames.at(i));
	}
	connect(ui->searchButton, &QPushButton::click, this, &SearchWidget::on_search_button_clicked);
}

SearchWidget::~SearchWidget()
{
}


void SearchWidget::closeEvent(QCloseEvent * event)
{
	emit closeSignal();
}

void SearchWidget::on_search_button_clicked()
{
	QString attriName = ui->attriNameComboBox->currentData().toString();
	QString attriValue = ui->attributeValueInputLine->text();
	if (attriValue != "") {
		if (ui->searchResultList->count()) {
			ui->searchResultList->clear();
		}
		QList<GeoFeature*> features = layer->search(layer, attriName, attriValue);
		for (int i = 0; i < features.size(); i++) {
			GeoFeature* feature = features.at(i);
			QMap<QString, QVariant>* map = feature->getAttributeMap();
			ui->searchResultList->addItem((*map)[attriName].toString());
			rowFeatureMap.insert(i, feature);
		}
		ui->resultNumLabel->setText(QString("共查询出") + features.size() + "条结果");
	}
	else {
		qDebug() << "not attribute value";
	}
}

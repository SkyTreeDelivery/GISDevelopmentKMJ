#pragma once

#include <QWidget>
#include "ui_SearchWidget.h"
#include "GeoLayer.h"

namespace Ui {
	class SearchWidget;
}

class SearchWidget : public QWidget
{
	Q_OBJECT

public:
	SearchWidget(GeoLayer* layer, QWidget *parent = nullptr);
	~SearchWidget();
	void on_search_button_clicked();

protected:
	void closeEvent(QCloseEvent *event);
private:
	Ui::SearchWidget* ui;
	GeoLayer* layer;
	QMap<int, GeoFeature*> rowFeatureMap;
signals:
	void closeSignal();
	void renderSelectedFeatures(QList<GeoFeature*> features);
};

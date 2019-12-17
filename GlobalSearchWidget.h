#pragma once

#include <QWidget>
#include <GeoMap.h>
namespace Ui { class GlobalSearchWidget; };

class GlobalSearchWidget : public QWidget
{
	Q_OBJECT

public:
	GlobalSearchWidget(GeoMap* map, QWidget *parent = Q_NULLPTR);
	~GlobalSearchWidget();

private:
	Ui::GlobalSearchWidget *ui;
	GeoMap* map;
	QMap<int, GeoFeature*> rowFeatureMap;

signals:

private slots:
	void on_searchButton_clicked();
};

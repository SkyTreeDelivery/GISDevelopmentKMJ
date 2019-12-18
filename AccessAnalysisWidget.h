#pragma once

#include <QWidget>
#include "GeoMap.h"
#include "PostgisHelper.h"
namespace Ui { class AccessAnalysisWidget; };

class AccessAnalysisWidget : public QWidget
{
	Q_OBJECT

public:
	AccessAnalysisWidget(GeoMap* map, QWidget *parent = Q_NULLPTR);
	~AccessAnalysisWidget();
	void initWidget();
	void connectDatabase(QString databaseUrl);

private:
	Ui::AccessAnalysisWidget *ui;
	GeoMap* map;
	GDALDataset* dataset;

public slots:
	void on_runTool();
};

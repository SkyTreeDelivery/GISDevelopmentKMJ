#ifndef WIDGET_H
#define WIDGET_H
#include <qtreewidget.h>
#include <qdialog.h>
#include "GeoMap.h"

namespace Ui {
class StyleWidget;
}

class StyleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StyleDialog(GeoLayer* layer, QWidget *parent = nullptr);
    ~StyleDialog(); 


private:
    Ui::StyleWidget *ui;
	QTreeWidget* styleTree;
	void initTree(GeoLayer* layer);
	GeoLayer* layer;
	LineSymbol* outline;
	FillSymbol* fill;
	QMap<QTreeWidgetItem*, int> itemType;
signals:
	void renderLayerSignal(GeoLayer* layer);
public slots:
	void on_item_clicked(QTreeWidgetItem* item);


};

#endif // WIDGET_H

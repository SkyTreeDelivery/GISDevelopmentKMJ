#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <qmenubar.h>
#include <qtreewidget.h>
#include <qplaintextedit.h>
#include "QtFunctionWidget.h"
#include "Symbol.h"
#include "StyleDialog.h"
#include "SearchWidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void addLayerInfo(GeoLayer* layer);
private:
	Ui::MainWindow *ui;
	QtFunctionWidget* glw;
	QString geojson_filename;
	QString shapefile_filename;

	QMenu *fileMenu;
	QMenu *glwMenu;
	QPlainTextEdit* outTextEdit;
	QTreeWidget* fileTree;
	void initRightClickMenu();
	void initMenuBar();
	QTreeWidgetItem* curItem;
	QMap<QTreeWidgetItem*, GeoLayer*> itemLayerMap; 
	SearchWidget* searchWidget;
	QTreeWidgetItem* searchWidgetOldOpenedItem;

	void initStyleWidget(QTreeWidgetItem* newItem);
	void initSearchWidget(QTreeWidgetItem* newItem);

signals:
	void addLayerSignal(GeoLayer* layer);
	void deleteLayerSignal(GeoLayer* layer);
	void zoomToLayerSignal(GeoLayer* layer);
	void setSymbolSignal(Symbol* symbol);
public slots:  //必须声明为slots才行
	void on_importGeoJson_action_triggered();
	void on_importShapeFile_action_triggered();
	void on_importFileFromPostgresql_action_triggered();

	void on_zoomToLayer_action_triggered();
	void on_deleteLayer_action_triggered();
	void on_setSLD_action_triggered();
	void on_setStyle_action_triggered();
	void on_identyfy_action_triggered();
	void on_search_action_triggered();

	void on_fileTree_contextMenu_request(const QPoint &pos);
	void on_glw_contextMenu_request(const QPoint &pos);
	void on_item_changed(QTreeWidgetItem *item);
	void on_searchWidget_closed();
	void on_style_Changed(GeoLayer* layer);
	void on_selectedStatus_Changed(QList<GeoFeature*> features);
	void on_textout_triggered(QString text);
};

#endif // MAINWINDOW_H

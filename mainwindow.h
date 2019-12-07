#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <qmenubar.h>
#include <qtreewidget.h>
#include <qplaintextedit.h>
#include "QtFunctionWidget.h"
#include "Symbol.h"
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

signals:
	void deleteLayerSignal(const QString& name);
	void zoomToLayerSignal(const QString& name);
	void setSymbolSignal(Symbol* symbol);
public slots:  //必须声明为slots才行
	void on_importGeoJson_action_triggered();
	void on_importShapeFile_action_triggered();
	void on_setSLD_action_triggered();
	void on_deleteLayer_action_triggered();
	void on_zoomToLayer_action_triggered();
	void on_fileTree_contextMenu_request(const QPoint &pos);
	void on_glw_contextMenu_request(const QPoint &pos);
};

#endif // MAINWINDOW_H

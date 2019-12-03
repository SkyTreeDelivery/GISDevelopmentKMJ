#ifndef WIDGET_H
#define WIDGET_H

#include <QString>
#include <QWidget>
#include <qmenubar.h>
#include <qtreewidget.h>
#include <qplaintextedit.h>
#include "QtFunctionWidget.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QMenuBar* genMenuBar();
	void addLayerInfo(GeoLayer* layer);


private:
    Ui::Widget *ui;
	QtFunctionWidget* glw;
    QString geojson_filename;
    QString shapefile_filename;

	QMenu *fileMenu; 
	QMenu *glwMenu; 
	QPlainTextEdit* outTextEdit;
	QTreeWidget* fileTree;
	void initRightClickMenu();
	QTreeWidgetItem* curItem;
	
signals:
	void deleteLayerSignal(const QString& name); 
	void zoomToLayerSignal(const QString& name); 

public slots:  //必须声明为slots才行
    void on_importGeoJson_action_triggered();
    void on_importShapeFile_action_triggered();
	void on_deleteLayer_action_triggered();
	void on_zoomToLayer_action_triggered();
	void on_fileTree_contextMenu_request(const QPoint &pos);
	void on_glw_contextMenu_request(const QPoint &pos);
};


#endif // WIDGET_H

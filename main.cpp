#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <qsurfaceformat.h>
#include "QtFunctionWidget.h"
#include <QVBoxLayout>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);

	/*QSurfaceFormat format;
	format.setSamples(16);
	

	TriangleWindow glWindow;
	WId gl_WId = glWindow.winId();
	glWindow.setFormat(format);
	glWindow.resize(640, 480);
	glWindow.setAnimating(true);
	glWindow.setParent();
    glWindow.show();
	*/
	
   /* Widget w;
	//WId mainw_WID = w.winId();
	MyGLWidget mygl;//(&w);
	//w.show();
	w.resize(400,800);
	mygl.show();*/

	
    /*QtFunctionWidget w1;
	 w1.show();*/
	MainWindow w;
	w.setWindowTitle("mainWindow");

	w.show();
    return a.exec();
}

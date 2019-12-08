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
	MainWindow w;
	w.setWindowTitle("mainWindow");

	w.show();
    return a.exec();
}

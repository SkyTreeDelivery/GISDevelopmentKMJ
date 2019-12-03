#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QWidget>
#include "GeoLayer.h"
#include <qopenglfunctions.h>

class util
{
public:
    util();
    static  QString util::openFileDialog(QWidget* widget, QString curPath);
    static GeoLayer* openGeoJson(QString path);
};

#endif // UTIL_H

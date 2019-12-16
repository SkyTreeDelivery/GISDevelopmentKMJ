#-------------------------------------------------
#
# Project created by QtCreator 2019-11-19T17:25:45
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GISystemDeveloper
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    util.cpp \
    GeoMap.cpp \
    GeoLayer.cpp \
    GeoObject.cpp \
    Render.cpp \
    GeoPolyline.cpp \
    GeoPolygon.cpp \
    GeoPoint.cpp

RESOURCES += qdarkstyle/style.qrc

HEADERS  += widget.h \
    util.h \
    GeoMap.h \
    GeoLayer.h \
    GeoObject.h \
    Render.h \
    GeoPoint.h \
    GeoPolygon.h \
    GeoPolyline.h

FORMS    += widget.ui


win32:CONFIG(release, debug|release): LIBS += -LE:/gdal/lib/ -lgdal_i
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/gdal/lib/ -lgdal_i

INCLUDEPATH += E:/gdal/include
DEPENDPATH += E:/gdal/include

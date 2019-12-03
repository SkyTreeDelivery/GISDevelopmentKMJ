#-------------------------------------------------
#
# Project created by QtCreator 2019-11-19T17:25:45
#
#-------------------------------------------------

QT       += core gui
QT       += widgets

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

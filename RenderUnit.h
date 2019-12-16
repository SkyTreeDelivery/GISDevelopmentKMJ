#pragma once
#include <qlist.h>
#include <qopenglvertexarrayobject.h>
#include <qcolor.h>
class RenderUnit
{
public:
	RenderUnit();
	~RenderUnit();
	QOpenGLVertexArrayObject* getVao();
	void setVao(QOpenGLVertexArrayObject* vao);
	QColor getColor();
	void setColor(QColor color);
private:
	QOpenGLVertexArrayObject* vao; //目前仅记录位置数据
	QColor color;  //配套使用的颜色
};


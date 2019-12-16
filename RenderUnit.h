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
	QOpenGLVertexArrayObject* vao; //Ŀǰ����¼λ������
	QColor color;  //����ʹ�õ���ɫ
};


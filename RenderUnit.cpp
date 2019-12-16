#include "RenderUnit.h"



RenderUnit::RenderUnit()
{
}


RenderUnit::~RenderUnit()
{
}

QOpenGLVertexArrayObject * RenderUnit::getVao()
{
	return vao;
}

void RenderUnit::setVao(QOpenGLVertexArrayObject * vao)
{
	this->vao = vao;
}

QColor RenderUnit::getColor()
{
	return color;
}

void RenderUnit::setColor(QColor color)
{
	this->color = color;
}

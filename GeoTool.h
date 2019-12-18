#pragma once
#include"COption.h"
#include "GeoLayer.h"
class GeoTool
{
public:
	GeoTool();
	~GeoTool();
	void setOption(COption *opt);
	COption* getOption();

	virtual GeoLayer* run_tool() = 0;  //�κμ��㶼����һ��ͼ����Ϊ���
protected:
	COption *option;
	
};


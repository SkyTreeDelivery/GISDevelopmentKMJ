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

	virtual GeoLayer* run_tool() = 0;  //任何计算都返回一个图层作为结果
protected:
	COption *option;
	
};


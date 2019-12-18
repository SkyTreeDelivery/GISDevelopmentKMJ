#pragma once
#include"COption.h"
class GeoTool
{
public:
	GeoTool();
	~GeoTool();
	void set_option(COption *opt);
	virtual int run_tool() = 0;
protected:
	COption *option;
};


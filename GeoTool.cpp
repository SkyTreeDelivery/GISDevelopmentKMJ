#include "GeoTool.h"

GeoTool::GeoTool():option(NULL)
{
}

GeoTool::~GeoTool()
{
}

void GeoTool::setOption(COption * opt)
{
	option = opt;
}

COption* GeoTool::getOption()
{
	return option;
}

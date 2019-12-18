#pragma once
#include "GeoTool.h"
class AccessibilityAnalysisTool :
	public GeoTool
{
public:
	AccessibilityAnalysisTool();
	~AccessibilityAnalysisTool(); 

	GeoLayer* run_tool();
};


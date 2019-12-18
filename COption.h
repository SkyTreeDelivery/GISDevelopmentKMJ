#pragma once
#include"qstring.h"
class COption
{
public:
	COption();
	~COption();

	int input_file;
	QString population_field;
	QString output_file;
	float output_cell_size;
	float search_radius;
	int area_unit;
	int output_value_type;
	int method_type;
};


#include "CGeoKernalDensTool.h"

CGeoKernalDensTool::CGeoKernalDensTool()
{
	GDALAllRegister();//注册所有驱动
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//支持中文路径
}

CGeoKernalDensTool::~CGeoKernalDensTool()
{
}

GeoLayer* CGeoKernalDensTool::run_tool()
{
	//根据用户的输入获得所有参数
	QList<GeoPoint*>points = getGeoPoints(layer);
	QRectF layerRect = layer->getRect();
	QList<float> populations = getPopulations(points, layer);
	QString output_file = option->output_file;
	float output_cell_size = getCellSize(layerRect);
	float search_radius = getSearchRadius(points, populations, layerRect);
	int area_unit = option->area_unit;
	int method = option->method_type;
	//调用核密度分析函数进行分析
	float **KDResult = KernelDensity(points, populations, output_cell_size,
		search_radius, area_unit, method, layerRect);
	//写入分析结果
	saveResult(output_file, KDResult, layerRect, output_cell_size);
	return NULL;
}

QList<GeoPoint*> CGeoKernalDensTool::getGeoPoints(GeoLayer * layer)
{
	QList<GeoPoint*>points;
	if (layer->getType() == EnumType::POINT)//仅支持点的核密度分析
	{
		QList<GeoFeature*>features = layer->getAllFeature();
		int featureNum = features.size();
		for (int i = 0; i < featureNum; i++)
		{
			points.append(static_cast<GeoPoint*>(features.at(i)->getGeometry()));
		}
	}
	else
		QMessageBox::critical(NULL, "Input Error", "Data Type Input Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	return points;
}

QList<float> CGeoKernalDensTool::getPopulations(QList<GeoPoint*> points,GeoLayer *layer)
{
	QList<float> populations;
	//如果用户未指定population字段，则将population全部赋值为1
	QString field = option->population_field;
	if (option->population_field.isNull())
	{
		for (int i = 0; i < points.size(); i++)
			populations.append(1.0f);
	}
	else
	{
		QList<QString> attris = layer->getAttributeNames();
		int attriNum = attris.size();
		for (int j = 0; j < points.size(); j++)//遍历所有要素
		{	
			QVariant var=layer->getFeatureAt(j)->getAttributeMap()->value(option->population_field);
			if (var.toFloat()>MINFLOAT)
				populations.append(var.toFloat());
			else
			{
				QMessageBox::critical(NULL, "Input Error", "Population Field Input Error", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
				exit(0);
			}
		}
	}
	return populations;
}

float CGeoKernalDensTool::getSearchRadius(QList<GeoPoint*> points, QList<float> populations,QRectF layerRect)
{
	float searchRadius;
	//如果用户未指定任何内容，则计算默认带宽
	if (option->search_radius < MINFLOAT)
	{
		float SD = 0, Dm = 0, n = 0;
		float Xsd = 0, Ysd = 0, Xmean = 0, Ymean = 0;
		float pointNum = points.size();
		//计算平均中心（加权）
		for (int i = 0; i < pointNum; i++)
		{
			Xmean += (points.at(i)->getXf())*populations.at(i);
			Ymean += (points.at(i)->getYf())*populations.at(i);
		}
		Xmean = Xmean / pointNum;
		Ymean = Ymean / pointNum;
		QPointF centerPoint = QPointF(Xmean, Ymean);//加权平均中心

		QList<float> disToCenter;//计算所有点（加权）到平均中心的距离
		for (int i = 0; i < pointNum; i++)
		{
			disToCenter.append(getDis(*points.at(i), centerPoint)*populations.at(i));//此处还要不要加权？
			n += populations.at(i);//n是点数（没权），population字段的总和（加权）
			Xsd += pow(points.at(i)->getXf() - Xmean, 2);
			Ysd += pow(points.at(i)->getYf() - Ymean, 2);
		}
		Dm = getMedian(disToCenter);//计算这些距离的中值
		SD = pow(Xsd*(1 / pointNum) + Ysd * (1 / pointNum), 0.5);
		float Dm1 = pow(1 / log(2), 0.5)*Dm;
		float minD = SD;
		if (minD > Dm1)
			minD = Dm1;
		searchRadius = 0.9*minD*pow(n, -0.2);
	}
	//如果用户指定内容，则直接返回
	else
		searchRadius = option->search_radius;
	return searchRadius;
}

float CGeoKernalDensTool::getCellSize(QRectF layerRect)
{
	float cellSize;
	//如果用户未指定任何内容，则象元大小会通过使用范围的宽度或高度中的较小值除以250来计算
	if (option->output_cell_size<MINFLOAT)
	{
		float min = abs(layerRect.height());
		if (min > abs(layerRect.width()))
			min = abs(layerRect.width());
		cellSize = min / 205;
	}
	//如果用户指定内容，则直接返回
	else
		cellSize = option->output_cell_size;
	return cellSize;
}

float CGeoKernalDensTool::getDis(GeoPoint pt1, QPointF pt2)
{
	float dis;
	float R = 6378137;
	//如果method_type为0则使用平面距离
	if (option->method_type == 0)
	{
		float dx = pt1.getXf() - pt2.x();
		float dy = pt1.getYf() - pt2.y();
		dis = pow(dx * dx + dy * dy, 0.5);
	}
	//如果method_type为1则使用大地线距离
	else if (option->method_type == 1)
	{
		float dB = (pt1.getYf() - pt2.x())*PI / 180;
		float dL = (pt1.getXf() - pt2.y())*PI / 180;
		float a = sinf(dL / 2)*sinf(dB / 2) + cosf(pt1.getXf()*PI / 180)*cosf(pt2.x()*PI / 180)*sinf(dL / 2)*sinf(dL / 2);
		float c = 2 * atan2f(pow(a, 0.5), pow((1 - a), 0.5));
		dis = R * c;
	}
	return dis;
}

float ** CGeoKernalDensTool::KernelDensity(QList<GeoPoint*> points, QList<float> populations,
	float cell_size, float search_radius, int area_unit, int method,QRectF layerRect)
{
	//根据cellsize得到行数和列数（取整）
	int nYSize, nXSize;
	nYSize = abs(layerRect.height()) / cell_size;
	nXSize = abs(layerRect.width()) / cell_size;

	//创建数据存储空间,并为每一个栅格赋初值0
	float **KDResult = new float *[nYSize];
	for (int y = 0; y < nYSize; y++)
	{
		KDResult[y] = new float[nXSize];
		for (int x = 0; x < nXSize; x++)
			KDResult[y][x] = 0.0f;
	}
	//创建每一栅格中心点对应的x坐标 y坐标矩阵
	float **LocX = new float*[nYSize];
	float **LocY = new float*[nYSize];
	for (int y = 0; y < nYSize; y++)
	{
		LocY[y] = new float[nXSize];
		LocX[y] = new float[nXSize];
		for (int x = 0; x < nXSize; x++)
		{
			LocX[y][x] = layerRect.left() + (0.5 + x)*cell_size;
			LocY[y][x] = layerRect.top() - (0.5 + y)*cell_size;
		}
	}

	if (option->method_type == 0)//输出值表示预测的密度值。这是默认设置。
	{
		//计算每一个栅格的数值,point的顺序与population的顺序一致
		int pointNum = points.size();
		for (int y = 0; y < nYSize; y++)
		{
			for (int x = 0; x < nXSize; x++)
			{
				QPointF gridPoint(LocX[y][x], LocY[y][x]);
				QList<float>pops;
				QList<float>dists;
				float pop, dist;
				for (int i = 0; i < pointNum; i++)//遍历每一个要素点，若在搜索半径范围内，则加入
				{
					dist = getDis(*points.at(i), gridPoint);
					pop = populations.at(i);
					if (dist < search_radius)
					{
						pops.append(pop);
						dists.append(dist);
					}
				}
				KDResult[y][x] = getDensity(search_radius, pops, dists);
			}
		}
	}
	else if (option->method_type == 1)//输出值表示每个像元中预测的现象数量。
	{
		int pointNum = points.size();
		for (int y = 0; y < nYSize; y++)
		{
			for (int x = 0; x < nXSize; x++)
			{
				QPointF gridPoint(LocX[y][x], LocY[y][x]);
				float count;
				float dist;
				for (int i = 0; i < pointNum; i++)//遍历每一个要素点，若在搜索半径范围内，则加入
				{
					dist = getDis(*points.at(i), gridPoint);
					if (dist < search_radius)
						count++;
				}
				KDResult[y][x] = count;
			}
		}
	}
	return KDResult;
}

float CGeoKernalDensTool::getDensity(float search_radius, QList<float> pops, QList<float> dists)
{
	//一共有n个要素点在所计算栅格的搜索半径内
	//pops为在半径内要素点的字段值，dists为计算栅格点与半径内要素点的距离
	int n = pops.size();
	float density = 0;
	if (n != 0)
	{
		float dens = 0;
		for (int i = 0; i < n; i++)
		{
			dens += (3 / PI)* pops.at(i)*pow((1 - pow((dists.at(i) / search_radius), 2)), 2);
		}
		density = (1 / pow(search_radius, 2))*dens*(1 / n);
	}
	return density;
}

void CGeoKernalDensTool::saveResult(QString output_file,float**KDResult,QRectF layerRect,float cell_size)
{
	//定义参数，配置图像信息
	QByteArray ba = output_file.toLocal8Bit();
	const char *outfilePath = ba.data();
	int nYSize = abs(layerRect.height()) / cell_size;
	int nXSize = abs(layerRect.width()) / cell_size;
	int nBands = 1;
	char **ppszOptions = NULL;
	char **raster_creation_options = CSLSetNameValue(ppszOptions, "BIGTIFF", "IF_NEEDED");//可以创建大文件

	//创建栅格数据集，如果创建后什么都不做，GDAL会自动写入0像素值
	GDALDataset *dataset;
	GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");//图像驱动
	dataset = driver->Create(outfilePath, nXSize, nYSize, nBands, GDT_Float32, raster_creation_options);

	//写入栅格数据信息
	dataset->RasterIO(GF_Write, 0, 0, nXSize, nYSize, KDResult, nXSize, nYSize, GDT_Float32, 1, NULL, 0, 0, 0);

	//关闭打开的文件
	GDALClose(dataset);
}

void CGeoKernalDensTool::setLayer(GeoLayer * layer1)
{
	layer = layer1;
}

float CGeoKernalDensTool::getMedian(QList<float> nums)
{
	float median;
	float temp;
	int size = nums.size();
	//冒泡排序法
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < size - 1 - i; j++)
		{
			if (nums.at(j) > nums.at(j + 1))
				nums.swapItemsAt(j, j+1);
		}
	}
	//寻找中值
	if (size % 2 == 0)
		median = 0.5*(nums.at(size*0.5) + nums.at(size*0.5 - 1));
	else
		median = nums.at(floor(size *0.5));
	return median;
}






#include "CGeoKernalDensTool.h"

CGeoKernalDensTool::CGeoKernalDensTool()
{
	GDALAllRegister();//ע����������
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//֧������·��
}

CGeoKernalDensTool::~CGeoKernalDensTool()
{
}

GeoLayer* CGeoKernalDensTool::run_tool()
{
	//�����û������������в���
	QList<GeoPoint*>points = getGeoPoints(layer);
	QRectF layerRect = layer->getRect();
	QList<float> populations = getPopulations(points, layer);
	QString output_file = option->output_file;
	float output_cell_size = getCellSize(layerRect);
	float search_radius = getSearchRadius(points, populations, layerRect);
	int area_unit = option->area_unit;
	int method = option->method_type;
	//���ú��ܶȷ����������з���
	float **KDResult = KernelDensity(points, populations, output_cell_size,
		search_radius, area_unit, method, layerRect);
	//д��������
	saveResult(output_file, KDResult, layerRect, output_cell_size);
	return NULL;
}

QList<GeoPoint*> CGeoKernalDensTool::getGeoPoints(GeoLayer * layer)
{
	QList<GeoPoint*>points;
	if (layer->getType() == EnumType::POINT)//��֧�ֵ�ĺ��ܶȷ���
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
	//����û�δָ��population�ֶΣ���populationȫ����ֵΪ1
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
		for (int j = 0; j < points.size(); j++)//��������Ҫ��
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
	//����û�δָ���κ����ݣ������Ĭ�ϴ���
	if (option->search_radius < MINFLOAT)
	{
		float SD = 0, Dm = 0, n = 0;
		float Xsd = 0, Ysd = 0, Xmean = 0, Ymean = 0;
		float pointNum = points.size();
		//����ƽ�����ģ���Ȩ��
		for (int i = 0; i < pointNum; i++)
		{
			Xmean += (points.at(i)->getXf())*populations.at(i);
			Ymean += (points.at(i)->getYf())*populations.at(i);
		}
		Xmean = Xmean / pointNum;
		Ymean = Ymean / pointNum;
		QPointF centerPoint = QPointF(Xmean, Ymean);//��Ȩƽ������

		QList<float> disToCenter;//�������е㣨��Ȩ����ƽ�����ĵľ���
		for (int i = 0; i < pointNum; i++)
		{
			disToCenter.append(getDis(*points.at(i), centerPoint)*populations.at(i));//�˴���Ҫ��Ҫ��Ȩ��
			n += populations.at(i);//n�ǵ�����ûȨ����population�ֶε��ܺͣ���Ȩ��
			Xsd += pow(points.at(i)->getXf() - Xmean, 2);
			Ysd += pow(points.at(i)->getYf() - Ymean, 2);
		}
		Dm = getMedian(disToCenter);//������Щ�������ֵ
		SD = pow(Xsd*(1 / pointNum) + Ysd * (1 / pointNum), 0.5);
		float Dm1 = pow(1 / log(2), 0.5)*Dm;
		float minD = SD;
		if (minD > Dm1)
			minD = Dm1;
		searchRadius = 0.9*minD*pow(n, -0.2);
	}
	//����û�ָ�����ݣ���ֱ�ӷ���
	else
		searchRadius = option->search_radius;
	return searchRadius;
}

float CGeoKernalDensTool::getCellSize(QRectF layerRect)
{
	float cellSize;
	//����û�δָ���κ����ݣ�����Ԫ��С��ͨ��ʹ�÷�Χ�Ŀ�Ȼ�߶��еĽ�Сֵ����250������
	if (option->output_cell_size<MINFLOAT)
	{
		float min = abs(layerRect.height());
		if (min > abs(layerRect.width()))
			min = abs(layerRect.width());
		cellSize = min / 205;
	}
	//����û�ָ�����ݣ���ֱ�ӷ���
	else
		cellSize = option->output_cell_size;
	return cellSize;
}

float CGeoKernalDensTool::getDis(GeoPoint pt1, QPointF pt2)
{
	float dis;
	float R = 6378137;
	//���method_typeΪ0��ʹ��ƽ�����
	if (option->method_type == 0)
	{
		float dx = pt1.getXf() - pt2.x();
		float dy = pt1.getYf() - pt2.y();
		dis = pow(dx * dx + dy * dy, 0.5);
	}
	//���method_typeΪ1��ʹ�ô���߾���
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
	//����cellsize�õ�������������ȡ����
	int nYSize, nXSize;
	nYSize = abs(layerRect.height()) / cell_size;
	nXSize = abs(layerRect.width()) / cell_size;

	//�������ݴ洢�ռ�,��Ϊÿһ��դ�񸳳�ֵ0
	float **KDResult = new float *[nYSize];
	for (int y = 0; y < nYSize; y++)
	{
		KDResult[y] = new float[nXSize];
		for (int x = 0; x < nXSize; x++)
			KDResult[y][x] = 0.0f;
	}
	//����ÿһդ�����ĵ��Ӧ��x���� y�������
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

	if (option->method_type == 0)//���ֵ��ʾԤ����ܶ�ֵ������Ĭ�����á�
	{
		//����ÿһ��դ�����ֵ,point��˳����population��˳��һ��
		int pointNum = points.size();
		for (int y = 0; y < nYSize; y++)
		{
			for (int x = 0; x < nXSize; x++)
			{
				QPointF gridPoint(LocX[y][x], LocY[y][x]);
				QList<float>pops;
				QList<float>dists;
				float pop, dist;
				for (int i = 0; i < pointNum; i++)//����ÿһ��Ҫ�ص㣬���������뾶��Χ�ڣ������
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
	else if (option->method_type == 1)//���ֵ��ʾÿ����Ԫ��Ԥ�������������
	{
		int pointNum = points.size();
		for (int y = 0; y < nYSize; y++)
		{
			for (int x = 0; x < nXSize; x++)
			{
				QPointF gridPoint(LocX[y][x], LocY[y][x]);
				float count;
				float dist;
				for (int i = 0; i < pointNum; i++)//����ÿһ��Ҫ�ص㣬���������뾶��Χ�ڣ������
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
	//һ����n��Ҫ�ص���������դ��������뾶��
	//popsΪ�ڰ뾶��Ҫ�ص���ֶ�ֵ��distsΪ����դ�����뾶��Ҫ�ص�ľ���
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
	//�������������ͼ����Ϣ
	QByteArray ba = output_file.toLocal8Bit();
	const char *outfilePath = ba.data();
	int nYSize = abs(layerRect.height()) / cell_size;
	int nXSize = abs(layerRect.width()) / cell_size;
	int nBands = 1;
	char **ppszOptions = NULL;
	char **raster_creation_options = CSLSetNameValue(ppszOptions, "BIGTIFF", "IF_NEEDED");//���Դ������ļ�

	//����դ�����ݼ������������ʲô��������GDAL���Զ�д��0����ֵ
	GDALDataset *dataset;
	GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");//ͼ������
	dataset = driver->Create(outfilePath, nXSize, nYSize, nBands, GDT_Float32, raster_creation_options);

	//д��դ��������Ϣ
	dataset->RasterIO(GF_Write, 0, 0, nXSize, nYSize, KDResult, nXSize, nYSize, GDT_Float32, 1, NULL, 0, 0, 0);

	//�رմ򿪵��ļ�
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
	//ð������
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < size - 1 - i; j++)
		{
			if (nums.at(j) > nums.at(j + 1))
				nums.swapItemsAt(j, j+1);
		}
	}
	//Ѱ����ֵ
	if (size % 2 == 0)
		median = 0.5*(nums.at(size*0.5) + nums.at(size*0.5 - 1));
	else
		median = nums.at(floor(size *0.5));
	return median;
}






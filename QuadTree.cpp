#include "QuadTree.h"

QuadTree::QuadTree()
{
	this->type = EnumType::indexMode::QUADTREE;
}

QuadTree::~QuadTree()
{

}

void QuadTree::CreateQuadTree(QRectF layerRect, QList<GeoFeature*> features)
{
	//初始化
	root = new QuadNode();
	int depth = treeDepth;

	//从根节点开始依次递归创建
	CreateQuadBranch(depth, layerRect, root, features);
}

void QuadTree::CreateQuadBranch(int depth, QRectF NodeRect, QuadNode *node, QList<GeoFeature*> features)
{
	if (depth - 1 != 0)
	{
		node->setNodeRect(NodeRect);
		QRectF childRect[4];
		node->splitRect4(NodeRect, childRect);
		for (int i = 0; i < 4; i++)
		{
			//创建四个节点
			QuadNode *childNode = new QuadNode();
			node->addChild(childNode);//添加孩子节点
			CreateQuadBranch(depth - 1, childRect[i], childNode, features);//递归创建所有的子节点
		}
	}
	else if (depth - 1 == 0)//如果孩子节点是叶节点，则储存空间要素
	{
		node->setNodeRect(NodeRect);
		QRectF childRect[4];
		node->splitRect4(NodeRect, childRect);
		for (int i = 0; i < 4; i++)
		{
			//创建四个节点
			QuadNode *childNode = new QuadNode();
			{
				childNode->setNodeRect(childRect[i]);
				int featureNum = features.size();
				for (int j = 0; j < featureNum; j++)//对每一个叶子节点遍历每一个要素
				{
					QRectF featureRect = features.at(j)->getGeometry()->getRect();
					if (childRect[i].intersects(featureRect))//如果该子节点的外界矩形与要素相交
					{
						childNode->addFeature(features.at(j));
					}
				}
			}
			node->addChild(childNode);//添加孩子节点
	    }
	}	
}

GeoFeature* QuadTree::SearchQuadTree(GeoPoint *point,int thresholed)
{
	return SearchQuadBranch(treeDepth, root, point, thresholed);
}

GeoFeature * QuadTree::SearchQuadBranch(int depth, QuadNode * node, GeoPoint *point, int threshole)
{
	GeoFeature* featureFound = NULL;
	if (depth!= 0)//非叶节点向下递归查询
	{
		if (node->getChildrenAt(0)->getNodeRect().contains(QPointF(point->getXf(), point->getYf())))
			featureFound = SearchQuadBranch(depth - 1, node->getChildrenAt(0), point, threshole);
		else if(node->getChildrenAt(1)->getNodeRect().contains(QPointF(point->getXf(), point->getYf())))
			featureFound = SearchQuadBranch(depth - 1, node->getChildrenAt(1), point, threshole);
		else if (node->getChildrenAt(2)->getNodeRect().contains(QPointF(point->getXf(), point->getYf())))
			featureFound = SearchQuadBranch(depth - 1, node->getChildrenAt(2), point, threshole);
		else if (node->getChildrenAt(3)->getNodeRect().contains(QPointF(point->getXf(), point->getYf())))
			featureFound = SearchQuadBranch(depth - 1, node->getChildrenAt(3), point, threshole);
	}
	else if (depth == 0)//叶节点查询并返回空间对象,终止递归查询
	{
		QList<GeoFeature*> features = node->getFeaturesAll();
		int featureNum = features.size();
		for (int i = 0; i < featureNum; i++)//遍历所有要素
		{
			GeoFeature *feature = features.at(i);
			GeoGeometry *geometry = feature->getGeometry();
			//如果点击的点与要素点小于一定的距离阈值，则返回该点
			if (geometry->getType() == EnumType::POINT)
			{
				GeoPoint *pt= static_cast<GeoPoint*>(geometry);
				if (pt->disToPoint(point) < threshole)//如果点到要素点的距离小于距离阈值，找到该要素
				{
					featureFound = feature;
					break;
				}
			}
			else if (geometry->getType() == EnumType::POLYLINE)
			{
				GeoPolyline *polyline = static_cast<GeoPolyline*>(geometry);
				if (polyline->disToPoint(point) < threshole)//如果点到要素线的距离小于距离阈值，找到该要素
				{
					featureFound = feature;
					break;
				}
			}
			//如果点击的点在多边形内则返回该多边形
			else if (geometry->getType() == EnumType::POLYGON)
			{
				GeoPolygon* polygon = static_cast<GeoPolygon*>(geometry);
				QVector<QPointF> polygonPoints;
				int pointNum = polygon->size();
				for (int j = 0; j < pointNum; j++)//将数据存储为Qpolygon可用的形式
				{
					QPointF pt;
					pt.setX(polygon->getPointAt(j)->getXf());
					pt.setY(polygon->getPointAt(j)->getYf());
					polygonPoints.append(pt);
				}
				if (QPolygonF(polygonPoints).containsPoint(QPointF(point->getXf(), point->getYf()),Qt::FillRule::OddEvenFill))//如果点包含在多边形内，找到该要素
				{
					featureFound = feature;
					break;
				}
			}
		}
	}
	return featureFound;
}

void QuadTree::ReleaseQuadTree()
{
	if (root == NULL)
		return;
	else
		ReleaseQuadBranch(treeDepth, root);
}

void QuadTree::ReleaseQuadBranch(int depth, QuadNode * node)
{
	if (depth != 0)
	{
		for (int i = 0; i < 4; i++)
		{
			ReleaseQuadBranch(depth - 1, node->getChildrenAt(i));
		}
		free(node);
		node = NULL;
	}
	node = NULL;
}







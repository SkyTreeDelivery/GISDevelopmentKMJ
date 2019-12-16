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
	//��ʼ��
	root = new QuadNode();
	int depth = treeDepth;

	//�Ӹ��ڵ㿪ʼ���εݹ鴴��
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
			//�����ĸ��ڵ�
			QuadNode *childNode = new QuadNode();
			node->addChild(childNode);//��Ӻ��ӽڵ�
			CreateQuadBranch(depth - 1, childRect[i], childNode, features);//�ݹ鴴�����е��ӽڵ�
		}
	}
	else if (depth - 1 == 0)//������ӽڵ���Ҷ�ڵ㣬�򴢴�ռ�Ҫ��
	{
		node->setNodeRect(NodeRect);
		QRectF childRect[4];
		node->splitRect4(NodeRect, childRect);
		for (int i = 0; i < 4; i++)
		{
			//�����ĸ��ڵ�
			QuadNode *childNode = new QuadNode();
			{
				childNode->setNodeRect(childRect[i]);
				int featureNum = features.size();
				for (int j = 0; j < featureNum; j++)//��ÿһ��Ҷ�ӽڵ����ÿһ��Ҫ��
				{
					QRectF featureRect = features.at(j)->getGeometry()->getRect();
					if (childRect[i].intersects(featureRect))//������ӽڵ����������Ҫ���ཻ
					{
						childNode->addFeature(features.at(j));
					}
				}
			}
			node->addChild(childNode);//��Ӻ��ӽڵ�
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
	if (depth!= 0)//��Ҷ�ڵ����µݹ��ѯ
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
	else if (depth == 0)//Ҷ�ڵ��ѯ�����ؿռ����,��ֹ�ݹ��ѯ
	{
		QList<GeoFeature*> features = node->getFeaturesAll();
		int featureNum = features.size();
		for (int i = 0; i < featureNum; i++)//��������Ҫ��
		{
			GeoFeature *feature = features.at(i);
			GeoGeometry *geometry = feature->getGeometry();
			//�������ĵ���Ҫ�ص�С��һ���ľ�����ֵ���򷵻ظõ�
			if (geometry->getType() == EnumType::POINT)
			{
				GeoPoint *pt= static_cast<GeoPoint*>(geometry);
				if (pt->disToPoint(point) < threshole)//����㵽Ҫ�ص�ľ���С�ھ�����ֵ���ҵ���Ҫ��
				{
					featureFound = feature;
					break;
				}
			}
			else if (geometry->getType() == EnumType::POLYLINE)
			{
				GeoPolyline *polyline = static_cast<GeoPolyline*>(geometry);
				if (polyline->disToPoint(point) < threshole)//����㵽Ҫ���ߵľ���С�ھ�����ֵ���ҵ���Ҫ��
				{
					featureFound = feature;
					break;
				}
			}
			//�������ĵ��ڶ�������򷵻ظö����
			else if (geometry->getType() == EnumType::POLYGON)
			{
				GeoPolygon* polygon = static_cast<GeoPolygon*>(geometry);
				QVector<QPointF> polygonPoints;
				int pointNum = polygon->size();
				for (int j = 0; j < pointNum; j++)//�����ݴ洢ΪQpolygon���õ���ʽ
				{
					QPointF pt;
					pt.setX(polygon->getPointAt(j)->getXf());
					pt.setY(polygon->getPointAt(j)->getYf());
					polygonPoints.append(pt);
				}
				if (QPolygonF(polygonPoints).containsPoint(QPointF(point->getXf(), point->getYf()),Qt::FillRule::OddEvenFill))//���������ڶ�����ڣ��ҵ���Ҫ��
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







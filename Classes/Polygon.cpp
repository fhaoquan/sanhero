#include "Polygon.h"


CPolygon::CPolygon(void)
{
}


CPolygon::~CPolygon(void)
{
}

int PointInPolygon (const IntPoint &pt, const Path &path)
{
	//returns 0 if false, +1 if true, -1 if pt ON polygon boundary
	//See "The Point in Polygon Problem for Arbitrary Polygons" by Hormann & Agathos
	//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.88.5498&rep=rep1&type=pdf
	int result = 0;
	size_t cnt = path.size();
	if (cnt < 3) return 0;
	IntPoint ip = path[0];
	for(size_t i = 1; i <= cnt; ++i)
	{
		IntPoint ipNext = (i == cnt ? path[0] : path[i]);
		if (ipNext.Y == pt.Y)
		{
			if ((ipNext.X == pt.X) || (ip.Y == pt.Y && 
				((ipNext.X > pt.X) == (ip.X < pt.X)))) return -1;
		}
		if ((ip.Y < pt.Y) != (ipNext.Y < pt.Y))
		{
			if (ip.X >= pt.X)
			{
				if (ipNext.X > pt.X) result = 1 - result;
				else
				{
					double d = (double)(ip.X - pt.X) * (ipNext.Y - pt.Y) - 
						(double)(ipNext.X - pt.X) * (ip.Y - pt.Y);
					if (!d) return -1;
					if ((d > 0) == (ipNext.Y > ip.Y)) result = 1 - result;
				}
			} else
			{
				if (ipNext.X > pt.X)
				{
					double d = (double)(ip.X - pt.X) * (ipNext.Y - pt.Y) - 
						(double)(ipNext.X - pt.X) * (ip.Y - pt.Y);
					if (!d) return -1;
					if ((d > 0) == (ipNext.Y > ip.Y)) result = 1 - result;
				}
			}
		}
		ip = ipNext;
	} 
	return result;
}

// 遍历全部三角形
//	return : bool 
// path顶点可以不按顺序
bool IsInPolygon(const IntPoint &pt, Path path)
{
	for(unsigned int i=0; i<path.size(); i++) {
		IntPoint pt1 = path[i];
		int id2 = i+1;
		if(id2 == path.size())
			id2 = 0;
		IntPoint pt2 = path[id2];
		printf("%d, %d\n", i, id2);
		for(unsigned j=0; j<path.size(); j++) {
			if(j != i && j != id2) {
				int id3 = j;
				IntPoint pt3 = path[id3];
				Path path2;
				path2.push_back(pt1);
				path2.push_back(pt2);
				path2.push_back(pt3);
				if(PointInPolygon(pt, path2))
					return true;
			}
		}
	}
	return false;
}

void MyTest()
{
	Path vec;
	// 逆时针顺序, 支持局部凹陷的多边形
	vec.push_back(IntPoint(10,10));
	vec.push_back(IntPoint(20,10));
	vec.push_back(IntPoint(22,13));
	vec.push_back(IntPoint(30,15));
	vec.push_back(IntPoint(20,20));
	vec.push_back(IntPoint(15,15));
	IntPoint pt(20,14);

	printf("%d\n", PointInPolygon(pt, vec));
	printf("测试遍历三角形\n");
	printf("%d\n", PointInPolygon(pt, vec));
	// 测试一个三角形的顺时针和逆时针
	vec.clear();
	vec.push_back(IntPoint(10,10));
	vec.push_back(IntPoint(20,10));
	vec.push_back(IntPoint(15,20));
	pt = IntPoint(15,15);
	printf("测试三角形逆时针\n");
	printf("%d\n", PointInPolygon(pt, vec));
	vec.clear();
	vec.push_back(IntPoint(10,10));
	vec.push_back(IntPoint(15,20));
	vec.push_back(IntPoint(20,10));
	printf("测试三角形顺时针\n");
	printf("%d\n", PointInPolygon(pt, vec));

	//IsInPolygon(pt, vec);
}

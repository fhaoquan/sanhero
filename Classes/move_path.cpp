#include "move_path.h"
#include "Polygon.h"
#include "HelloWorldScene.h"

namespace MovePath{

const int aaa = 1;
Vec2 get_multi_points_center(const vector<Vec2>& points) {
	Vec2 pre_point = points[0];
	for (unsigned i = 1; i<points.size(); i++) {
		float x = points[i].x;
		float y = points[i].y;
		float x1 = pre_point.x;
		float y1 = pre_point.y;
		float mid_x, mid_y;
		if (x > x1)
			mid_x = x - (x - x1) / 2;
		else
		{
			mid_x = x1 - (x1 - x) / 2;
		}
		if (y > y1)
			mid_y = y - (y - y1) / 2;
		else
		{
			mid_y = y1 - (y1 - y) / 2;
		}
		pre_point.x = mid_x;
		pre_point.y = mid_y;
		
	}
	return pre_point;
}
		
//def getParallelPoint(pt1, pt_org, pt_dest) :
//"""坐标上求平移点 pt1平移之前的点"""
//pt_offset_x = pt_org[0] - pt1[0]
//pt_offset_y = pt_org[1] - pt1[1]
//return pt_dest[0] - pt_offset_x, pt_dest[1] - pt_offset_y
Vec2 getParallelPoint(Vec2 pt1, Vec2 pt_org, Vec2 pt_dest) {
	float pt_offset_x = pt_org.x - pt1.x;
	float pt_offset_y = pt_org.y - pt1.y;
	return Vec2(pt_dest.x - pt_offset_x, pt_dest.y - pt_offset_y);
}

float getDistance(Vec2 p1, Vec2 p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

//def move_path(points, pos) :
//#计算n个点的中心点
//pt, d = get_multi_points_center(points)
//#判断点是否在多边形内
//is_in_scope = False
//dest_pts = []
//for cur_pt in points :
//if is_in_scope :
//dest_pts.append(pos)
//else :
//dest_pt = getParallelPoint(cur_pt, pt, pos)
//dest_pts.append(dest_pt)
//return dest_pts
vector<Vec2> Move_Path(const vector<Vec2>& points, Vec2 pos) {
	assert(points.size() > 0);
	Vec2 pt_center = get_multi_points_center(points);
	vector<Vec2> vec;
	Path path;
	for (unsigned i = 0; i < points.size(); i++) {
		path.push_back(IntPoint(points[i].x, points[i].y) );
	}
	IntPoint pt(pos.x, pos.y);
	bool is_in_polygon = IsInPolygon(pt, path);
	if (!is_in_polygon) {
		for (unsigned i = 0; i < points.size(); i++) {
			Vec2 dest_pt = getParallelPoint(points[i], pt_center, pos);
			vec.push_back(dest_pt);
		}
	}
	else
	{
		for (unsigned i = 0; i < points.size(); i++) {
			vec.push_back(pos);
		}
	}
	return vec;
}

//vector<Vec2> MovePath::GetTargetPoss(vector<Vec2> vecUser, Vec2 posTarget)
//{
//	vector<Vec2> vec;
//
//	return vec;
//}

// 0ad的寻路实现
void(*fn_computePath)(float x, float y, float x1, float y1, float* pUnits, int size_units, int unit_clearance, float* pOutPaths, int* size_path) = nullptr;

FindPath::FindPath(HelloWorld* pLayer)
{
	if (fn_computePath == nullptr) {
		HMODULE h = LoadLibraryA("lib0ad.dll");
		*((DWORD**)&fn_computePath) = (DWORD*)GetProcAddress(h, "computePath");
	}
	convertObstacleTo0ad(pLayer);
	_pLayer = pLayer;
}

FindPath::~FindPath()
{

}

vector<Vec2> FindPath::getPaths(Vec2 startPos, Vec2 endPos)
{
	vector<Vec2> vec;
	//在瓦片坐标里寻好路再转换为视图坐标
	int width = 32;
	for (unsigned int i = 0; i < _vecRect.size(); i++) {
		Rect& rc = _vecRect[i];
		int width_number = rc.size.width / width;
		int height_number = rc.size.height / width;
		for (int j = 0; j < width_number; j++) {
			for (int k = 0; k < height_number; k++) {
				float x, y;
				x = rc.origin.x + width / 2 + j*width;
				y = rc.origin.y + width / 2+ k*width;
				vec.push_back(Vec2(x, y));
			}
		}
	}

	if (vec.size() == 0)
		return vec;
	float* pUnits = new float[vec.size()*2];
	int j = 0;
	for (unsigned i = 0; i < vec.size(); i++) {
		pUnits[j++] = vec[i].x;
		pUnits[j++] = vec[i].y;
	}

	float x, y, x1, y1;
	x = startPos.x;
	y = startPos.y;
	x1 = endPos.x;
	y1 = endPos.y;
	float pOut[1000];
	int nOut = 0;
	fn_computePath(x, y, x1, y1, pUnits, j, width / 2, pOut, &nOut);
	delete[] pUnits;
	vec.clear();
	vec.push_back(Vec2(x1, y1));
	for (int i = 0; i < nOut; i++) {
		float x = pOut[i++];
		float y = pOut[i];
		vec.push_back(Vec2(x, y));
	}
	vec.push_back(Vec2(x, y));

	// 最后把寻路路径转换为视图坐标
	for (unsigned i = 0; i < vec.size(); i++) {
		vec[i] = _pLayer->TileConvertToViewPos(vec[i]);
	}
	return vec;
}

void FindPath::convertObstacleTo0ad(HelloWorld* pLayer)
{
	_vecRect = pLayer->getTileObstacle();

}

};

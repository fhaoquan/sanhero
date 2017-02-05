#pragma once

#include <vector>

class CPolygon
{
public:
	CPolygon(void);
	~CPolygon(void);
};

class IntPoint
{
public:
	int X;
	int Y;
	IntPoint(float x, float y) {
		X = (int)x*100;
		Y = (int)y*100;
	}
};

typedef std::vector<IntPoint> Path;
typedef std::vector< Path > Paths;

bool IsInPolygon(const IntPoint &pt, Path path);
int PointInPolygon (const IntPoint &pt, const Path &path);

void MyTest();
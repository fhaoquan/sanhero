// test_lib0ad.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "simulation2/helpers/0ad_path_finder.h"
#include <windows.h>
#include <fstream>
#include <time.h>

void(*fn_computePath)(float x, float y, float x1, float y1, float* pUnits, int size_units, int unit_clearance, float* pOutPaths, int* size_path) = NULL;
int main(int argc, char *argv[])
{
	HMODULE h = LoadLibraryA("C:\\work\\0ad\\binaries\\system\\lib0ad.dll");
	printf("last error %d\n", GetLastError());
	printf("%08x\n", h);
	if (!h) {
		printf("LoadLibraryA failed.\n");
		return 1;
	}
	*((DWORD**)&fn_computePath) = (DWORD*)GetProcAddress(h, "computePath");
	printf("%08x\n", fn_computePath);
	if (!fn_computePath) {
		printf("GetProcAddress failed.\n");
		return 1;
	}
	std::ofstream outfile("pather_struct.csv", std::ios::out);
	float units[1000];
	int size_unit = 200;
	srand(GetTickCount());
	for (int i = 0; i < 200; i++) {
		float x = 1.5f*48.0f * rand() / (float)RAND_MAX;
		float y = 1.5f*48.0f * rand() / (float)RAND_MAX;
		units[i++] = x;
		units[i] = y;
		outfile << x << " " << y << "\n";
	}
	outfile.close();

	float pOut[1000];
	int size_out = 0;
	float x0 = 17.23f;
	float y0 = 10.2f;
	float x1 = 48.0f;
	float y1 = 48.0f;
	std::ofstream outfile2("pather_path.csv", std::ios::out);
	outfile2 << x1 << " " << y1 << "\n";
	fn_computePath(x0,y0,x1,y1, units, size_unit, 2, pOut, &size_out);
	for (int i = 0; i < size_out; i++) {
		float x = pOut[i++];
		float y = pOut[i];
		outfile2 << x << " " << y << "\n";
	}
	outfile2 << x0 << " " << y0 << "\n";
	outfile2.close();
	printf("path size %d", size_out);
	return 0;
}


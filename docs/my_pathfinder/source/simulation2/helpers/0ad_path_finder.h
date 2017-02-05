#ifndef __0AD_PATH_FINDER_H__
#define __0AD_PATH_FINDER_H__

#ifdef _DLL
extern "C" __declspec(dllexport) void computePath(float x, float y, float x1, float y1, float* pUnits, int size_units, int unit_clearance, float* pOutPaths, int* size_path);
#else
extern "C" __declspec(dllimport) void computePath(float x, float y, float x1, float y1, float* pUnits, int size_units, int unit_clearance, float* pOutPaths, int* size_path);
#endif

#endif
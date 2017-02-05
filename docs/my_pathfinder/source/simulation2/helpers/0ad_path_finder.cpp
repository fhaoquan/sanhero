#include "precompiled.h"
#include "0ad_path_finder.h"
#include "simulation2/components/tests/test_Pathfinder.h"
#include "scriptinterface/ScriptEngine.h"
#include "lib/sysdep/os/win/winit.h"
#include "myPathFinder.h"
//void computePath(float x, float y, float x1, float y1, float* pUnits, int size_units, int unit_clearance, float* pOutPaths, int* size_path) {
//
//}
ICmpObstructionManager* CreateCCmpObstructionManager();
ICmpPathfinder* CreateCCmpPathfinder();

void computePath(float x, float y, float x1, float y1, float* pUnits, int size_units,int unit_clearance, float* pOutPaths, int* size_path) {
	//CTerrain terrain;
	//terrain.Initialize(5, NULL);

	//if (g_ScriptRuntime.get() == nullptr) {
	//	winit_CallInitFunctions();
	//	timer_LatchStartTime();
	//	ThreadUtil::SetMainThread();
	//	g_VFS = CreateVfs(20 * MiB);
	//	g_VFS->Mount(L"", DataDir() / "mods" / "mod", VFS_MOUNT_MUST_EXIST);
	//	g_VFS->Mount(L"", DataDir() / "mods" / "public", VFS_MOUNT_MUST_EXIST, 1); // ignore directory-not-found errors
	//	g_VFS->Mount(L"cache/", DataDir() / "cache");

	//	CXeromyces::Startup();
	//	ScriptEngine* m_ScriptEngine = new ScriptEngine;
	//	g_ScriptRuntime = ScriptInterface::CreateRuntime();
	//}
	//CSimulation2 sim2(NULL, g_ScriptRuntime, &terrain);
	//sim2.LoadDefaultScripts();
	//sim2.ResetState();

	const entity_pos_t range = entity_pos_t::FromInt(TERRAIN_TILE_SIZE * 12);

	//CmpPtr<ICmpObstructionManager> cmpObstructionMan(sim2, SYSTEM_ENTITY);
	//CmpPtr<ICmpPathfinder> cmpPathfinder(sim2, SYSTEM_ENTITY);
	ICmpObstructionManager* cmpObstructionMan = CreateCCmpObstructionManager();
	cmpObstructionMan->Init(NULL);
	MyPathFinder* p = new MyPathFinder();
	p->Init2();
	ICmpPathfinder* cmpPathfinder = p;

	for (size_t i = 0; i < size_units; ++i)
	{
		fixed x = fixed::FromFloat(pUnits[i++]);
		fixed z = fixed::FromFloat(pUnits[i]);
		cmpObstructionMan->AddUnitShape(INVALID_ENTITY, x, z, fixed::FromInt(unit_clearance), 0, INVALID_ENTITY);
	}
	//for (size_t i = 10; i < 40; ++i)
	//{
	//	fixed x = fixed::FromFloat(20);
	//	fixed z = fixed::FromFloat(i);
	//	cmpObstructionMan->AddUnitShape(INVALID_ENTITY, x, z, fixed::FromInt(2), 0, INVALID_ENTITY);
	//}

	entity_pos_t start_pos = entity_pos_t::FromInt(0);
	start_pos = range / 4;
	NullObstructionFilter filter;
	PathGoal goal = { PathGoal::POINT, entity_pos_t::FromFloat(x1), entity_pos_t::FromFloat(y1)};
	//PathGoal goal = { PathGoal::POINT, range, range};
	WaypointPath path;
	p->UpdateGrid2(80, cmpObstructionMan);
	cmpPathfinder->ComputeShortPath(filter, entity_pos_t::FromFloat(x), entity_pos_t::FromFloat(y), fixed::FromInt(unit_clearance),
		range, goal, 0, path);
	int j = 0;
	for (size_t i = 0; i < path.m_Waypoints.size(); ++i) {
		pOutPaths[j++] = path.m_Waypoints[i].x.ToFloat();
		pOutPaths[j++] = path.m_Waypoints[i].z.ToFloat();
	}
	*size_path = j;
}
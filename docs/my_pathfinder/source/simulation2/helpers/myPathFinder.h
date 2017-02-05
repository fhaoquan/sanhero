#ifndef __MYPATHFINDER_H__
#define __MYPATHFINDER_H__

#include "simulation2/components/CCmpPathfinder_Common.h"

__interface IPathFinder2
{
	virtual void Init2() = 0;
	virtual void UpdateGrid2(u16 tile_side, ICmpObstructionManager* cmpObstructionManager) = 0;
};
class MyPathFinder : public CCmpPathfinder, public IPathFinder2
{
public:
	virtual void Init2();
	virtual void UpdateGrid2(u16 tile_side, ICmpObstructionManager* cmpObstructionManager);
	void TerrainUpdateHelper(bool expandPassability);
	ICmpObstructionManager* m_cmpObstructionManager;
	virtual Grid<u16> ComputeShoreGrid(bool expandOnWater = false);
	virtual void ComputeShortPath(const IObstructionTestFilter& filter, entity_pos_t x0, entity_pos_t z0, entity_pos_t clearance, entity_pos_t range, const PathGoal& goal, pass_class_t passClass, WaypointPath& ret);
};
#endif
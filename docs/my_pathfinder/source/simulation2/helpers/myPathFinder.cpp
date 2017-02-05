#include "precompiled.h"
#include "myPathFinder.h"

void MyPathFinder::Init2()
{
	m_MapSize = 0;
	m_Grid = NULL;
	m_TerrainOnlyGrid = NULL;

	m_ObstructionsDirty.Clean();
	m_PreserveUpdateInformations = false;

	m_NextAsyncTicket = 1;

	m_DebugOverlay = false;
	m_AtlasOverlay = NULL;

	m_SameTurnMovesCount = 0;
	m_MaxSameTurnMoves = 64;

	//CParamNode node;
	//node.
	//const CParamNode::ChildrenMap& passClasses = externalParamNode.GetChild("Pathfinder").GetChild("PassabilityClasses").GetChildren();
	//for (CParamNode::ChildrenMap::const_iterator it = passClasses.begin(); it != passClasses.end(); ++it)
	//{
	//	std::string name = it->first;
	//	ENSURE((int)m_PassClasses.size() <= PASS_CLASS_BITS);
	//	pass_class_t mask = PASS_CLASS_MASK_FROM_INDEX(m_PassClasses.size());
	//	m_PassClasses.push_back(PathfinderPassability(mask, it->second));
	//	m_PassClassMasks[name] = mask;
	//}
}

void MyPathFinder::UpdateGrid2(u16 tile_side, ICmpObstructionManager* cmpObstructionManager)
{
	m_cmpObstructionManager = cmpObstructionManager;
	if (!m_PreserveUpdateInformations)
		m_ObstructionsDirty.Clean();
	else
		m_PreserveUpdateInformations = false; // Next time will be a regular update

	u16 terrainSize = tile_side;
	if (terrainSize == 0)
		return;

	// If the terrain was resized then delete the old grid data
	if (m_Grid && m_MapSize != terrainSize)
	{
		SAFE_DELETE(m_Grid);
		SAFE_DELETE(m_TerrainOnlyGrid);
	}

	// Initialise the terrain data when first needed
	if (!m_Grid)
	{
		m_MapSize = terrainSize;
		m_Grid = new Grid<NavcellData>(m_MapSize * Pathfinding::NAVCELLS_PER_TILE, m_MapSize * Pathfinding::NAVCELLS_PER_TILE);
		m_TerrainOnlyGrid = new Grid<NavcellData>(m_MapSize * Pathfinding::NAVCELLS_PER_TILE, m_MapSize * Pathfinding::NAVCELLS_PER_TILE);

		m_ObstructionsDirty.dirty = true;
		m_ObstructionsDirty.globallyDirty = true;
		m_ObstructionsDirty.globalRecompute = true;

		m_TerrainDirty = true;
	}

	cmpObstructionManager->UpdateInformations(m_ObstructionsDirty);

	if (!m_ObstructionsDirty.dirty && !m_TerrainDirty)
		return;

	// If the terrain has changed, recompute m_Grid
	// Else, use data from m_TerrainOnlyGrid and add obstructions
	if (m_TerrainDirty)
	{
		TerrainUpdateHelper(true);

		*m_Grid = *m_TerrainOnlyGrid;

		m_TerrainDirty = false;
		m_ObstructionsDirty.globalRecompute = true;
		m_ObstructionsDirty.globallyDirty = true;
	}
	else if (m_ObstructionsDirty.globalRecompute)
	{
		ENSURE(m_Grid->m_W == m_TerrainOnlyGrid->m_W && m_Grid->m_H == m_TerrainOnlyGrid->m_H);
		memcpy(m_Grid->m_Data, m_TerrainOnlyGrid->m_Data, (m_Grid->m_W)*(m_Grid->m_H)*sizeof(NavcellData));

		m_ObstructionsDirty.globallyDirty = true;
	}
	else
	{
		ENSURE(m_Grid->m_W == m_ObstructionsDirty.dirtinessGrid.m_W && m_Grid->m_H == m_ObstructionsDirty.dirtinessGrid.m_H);
		ENSURE(m_Grid->m_W == m_TerrainOnlyGrid->m_W && m_Grid->m_H == m_TerrainOnlyGrid->m_H);

		for (u16 i = 0; i < m_ObstructionsDirty.dirtinessGrid.m_W; ++i)
			for (u16 j = 0; j < m_ObstructionsDirty.dirtinessGrid.m_H; ++j)
				if (m_ObstructionsDirty.dirtinessGrid.get(i, j) == 1)
					m_Grid->set(i, j, m_TerrainOnlyGrid->get(i, j));
	}

	// Add obstructions onto the grid
	cmpObstructionManager->Rasterize(*m_Grid, m_PassClasses, m_ObstructionsDirty.globalRecompute);

	// Update the long-range pathfinder
	if (m_ObstructionsDirty.globallyDirty)
	{
		std::map<std::string, pass_class_t> nonPathfindingPassClasses, pathfindingPassClasses;
		GetPassabilityClasses(nonPathfindingPassClasses, pathfindingPassClasses);
		m_LongPathfinder.Reload(m_Grid, nonPathfindingPassClasses, pathfindingPassClasses);
	}
	else
		m_LongPathfinder.Update(m_Grid, m_ObstructionsDirty.dirtinessGrid);
}

static void ExpandImpassableCells(Grid<NavcellData>& grid, u16 clearance, pass_class_t mask)
{

	u16 w = grid.m_W;
	u16 h = grid.m_H;

	// First expand impassable cells horizontally into a temporary 1-bit grid
	Grid<u8> tempGrid(w, h);
	for (u16 j = 0; j < h; ++j)
	{
		// New cell (i,j) is blocked if (i',j) blocked for any i-clearance <= i' <= i+clearance

		// Count the number of blocked cells around i=0
		u16 numBlocked = 0;
		for (u16 i = 0; i <= clearance && i < w; ++i)
			if (!IS_PASSABLE(grid.get(i, j), mask))
				++numBlocked;

		for (u16 i = 0; i < w; ++i)
		{
			// Store a flag if blocked by at least one nearby cell
			if (numBlocked)
				tempGrid.set(i, j, 1);

			// Slide the numBlocked window along:
			// remove the old i-clearance value, add the new (i+1)+clearance
			// (avoiding overflowing the grid)
			if (i >= clearance && !IS_PASSABLE(grid.get(i - clearance, j), mask))
				--numBlocked;
			if (i + 1 + clearance < w && !IS_PASSABLE(grid.get(i + 1 + clearance, j), mask))
				++numBlocked;
		}
	}

	for (u16 i = 0; i < w; ++i)
	{
		// New cell (i,j) is blocked if (i,j') blocked for any j-clearance <= j' <= j+clearance
		// Count the number of blocked cells around j=0
		u16 numBlocked = 0;
		for (u16 j = 0; j <= clearance && j < h; ++j)
			if (tempGrid.get(i, j))
				++numBlocked;

		for (u16 j = 0; j < h; ++j)
		{
			// Add the mask if blocked by at least one nearby cell
			if (numBlocked)
				grid.set(i, j, grid.get(i, j) | mask);

			// Slide the numBlocked window along:
			// remove the old j-clearance value, add the new (j+1)+clearance
			// (avoiding overflowing the grid)
			if (j >= clearance && tempGrid.get(i, j - clearance))
				--numBlocked;
			if (j + 1 + clearance < h && tempGrid.get(i, j + 1 + clearance))
				++numBlocked;
		}
	}
}

void MyPathFinder::TerrainUpdateHelper(bool expandPassability)
{
	//CmpPtr<ICmpObstructionManager> cmpObstructionManager(GetSimContext(), SYSTEM_ENTITY);

	ICmpObstructionManager* cmpObstructionManager = m_cmpObstructionManager;
	//CmpPtr<ICmpWaterManager> cmpWaterManager(GetSimContext(), SYSTEM_ENTITY);
	//ICmpWaterManager* cmpWaterManager = NULL;
	//CmpPtr<ICmpTerrain> cmpTerrain(GetSimContext(), SYSTEM_ENTITY);
	//CTerrain& terrain = GetSimContext().GetTerrain();
	CTerrain terrain;
	terrain.Initialize(5, NULL);

	u16 terrainSize = 80;
	if (terrainSize == 0)
		return;

	if (!m_TerrainOnlyGrid || m_MapSize != terrainSize)
	{
		m_MapSize = terrainSize;

		SAFE_DELETE(m_TerrainOnlyGrid);
		m_TerrainOnlyGrid = new Grid<NavcellData>(m_MapSize * Pathfinding::NAVCELLS_PER_TILE, m_MapSize * Pathfinding::NAVCELLS_PER_TILE);
	}

	Grid<u16> shoreGrid = ComputeShoreGrid();

	// Compute initial terrain-dependent passability
	for (int j = 0; j < m_MapSize * Pathfinding::NAVCELLS_PER_TILE; ++j)
	{
		for (int i = 0; i < m_MapSize * Pathfinding::NAVCELLS_PER_TILE; ++i)
		{
			// World-space coordinates for this navcell
			fixed x, z;
			Pathfinding::NavcellCenter(i, j, x, z);

			// Terrain-tile coordinates for this navcell
			int itile = i / Pathfinding::NAVCELLS_PER_TILE;
			int jtile = j / Pathfinding::NAVCELLS_PER_TILE;

			// Gather all the data potentially needed to determine passability:

			fixed height = terrain.GetExactGroundLevelFixed(x, z);

			fixed water = fixed::FromInt(0);
			//if (cmpWaterManager)
			//	water = cmpWaterManager->GetWaterLevel(x, z);

			fixed depth = water - height;

			// Exact slopes give kind of weird output, so just use rough tile-based slopes
			fixed slope = terrain.GetSlopeFixed(itile, jtile);

			// Get world-space coordinates from shoreGrid (which uses terrain tiles)
			fixed shoredist = fixed::FromInt(shoreGrid.get(itile, jtile)).MultiplyClamp(TERRAIN_TILE_SIZE);

			// Compute the passability for every class for this cell
			NavcellData t = 0;
			for (PathfinderPassability& passability : m_PassClasses)
				if (!passability.IsPassable(depth, slope, shoredist))
					t |= passability.m_Mask;

			m_TerrainOnlyGrid->set(i, j, t);
		}
	}

	// Compute off-world passability
	// WARNING: CCmpRangeManager::LosIsOffWorld needs to be kept in sync with this
	const int edgeSize = 3 * Pathfinding::NAVCELLS_PER_TILE; // number of tiles around the edge that will be off-world

	NavcellData edgeMask = 0;
	for (PathfinderPassability& passability : m_PassClasses)
		edgeMask |= passability.m_Mask;

	int w = m_TerrainOnlyGrid->m_W;
	int h = m_TerrainOnlyGrid->m_H;

	if (cmpObstructionManager->GetPassabilityCircular())
	{
		for (int j = 0; j < h; ++j)
		{
			for (int i = 0; i < w; ++i)
			{
				// Based on CCmpRangeManager::LosIsOffWorld
				// but tweaked since it's tile-based instead.
				// (We double all the values so we can handle half-tile coordinates.)
				// This needs to be slightly tighter than the LOS circle,
				// else units might get themselves lost in the SoD around the edge.

				int dist2 = (i * 2 + 1 - w)*(i * 2 + 1 - w)
					+ (j * 2 + 1 - h)*(j * 2 + 1 - h);

				if (dist2 >= (w - 2 * edgeSize) * (h - 2 * edgeSize))
					m_TerrainOnlyGrid->set(i, j, m_TerrainOnlyGrid->get(i, j) | edgeMask);
			}
		}
	}
	else
	{
		for (u16 j = 0; j < h; ++j)
			for (u16 i = 0; i < edgeSize; ++i)
				m_TerrainOnlyGrid->set(i, j, m_TerrainOnlyGrid->get(i, j) | edgeMask);
		for (u16 j = 0; j < h; ++j)
			for (u16 i = w - edgeSize + 1; i < w; ++i)
				m_TerrainOnlyGrid->set(i, j, m_TerrainOnlyGrid->get(i, j) | edgeMask);
		for (u16 j = 0; j < edgeSize; ++j)
			for (u16 i = edgeSize; i < w - edgeSize + 1; ++i)
				m_TerrainOnlyGrid->set(i, j, m_TerrainOnlyGrid->get(i, j) | edgeMask);
		for (u16 j = h - edgeSize + 1; j < h; ++j)
			for (u16 i = edgeSize; i < w - edgeSize + 1; ++i)
				m_TerrainOnlyGrid->set(i, j, m_TerrainOnlyGrid->get(i, j) | edgeMask);
	}

	if (!expandPassability)
		return;

	// Expand the impassability grid, for any class with non-zero clearance,
	// so that we can stop units getting too close to impassable navcells.
	// Note: It's not possible to perform this expansion once for all passabilities
	// with the same clearance, because the impassable cells are not necessarily the 
	// same for all these passabilities.
	for (PathfinderPassability& passability : m_PassClasses)
	{
		if (passability.m_Clearance == fixed::Zero())
			continue;

		int clearance = (passability.m_Clearance / Pathfinding::NAVCELL_SIZE).ToInt_RoundToInfinity();
		ExpandImpassableCells(*m_TerrainOnlyGrid, clearance, passability.m_Mask);
	}
}

Grid<u16> MyPathFinder::ComputeShoreGrid(bool expandOnWater /*= false*/)
{
	//CmpPtr<ICmpWaterManager> cmpWaterManager(GetSystemEntity());

	// TODO: these bits should come from ICmpTerrain
	//CTerrain& terrain = GetSimContext().GetTerrain();

	// avoid integer overflow in intermediate calculation
	const u16 shoreMax = 32767;

	// First pass - find underwater tiles
	Grid<u8> waterGrid(m_MapSize, m_MapSize);
	for (u16 j = 0; j < m_MapSize; ++j)
	{
		for (u16 i = 0; i < m_MapSize; ++i)
		{
			fixed x, z;
			Pathfinding::TileCenter(i, j, x, z);

			//bool underWater = cmpWaterManager && (cmpWaterManager->GetWaterLevel(x, z) > terrain.GetExactGroundLevelFixed(x, z));
			bool underWater = false;
			waterGrid.set(i, j, underWater ? 1 : 0);
		}
	}

	// Second pass - find shore tiles
	Grid<u16> shoreGrid(m_MapSize, m_MapSize);
	for (u16 j = 0; j < m_MapSize; ++j)
	{
		for (u16 i = 0; i < m_MapSize; ++i)
		{
			// Find a land tile
			if (!waterGrid.get(i, j))
			{
				// If it's bordered by water, it's a shore tile
				if ((i > 0 && waterGrid.get(i - 1, j)) || (i > 0 && j < m_MapSize - 1 && waterGrid.get(i - 1, j + 1)) || (i > 0 && j > 0 && waterGrid.get(i - 1, j - 1))
					|| (i < m_MapSize - 1 && waterGrid.get(i + 1, j)) || (i < m_MapSize - 1 && j < m_MapSize - 1 && waterGrid.get(i + 1, j + 1)) || (i < m_MapSize - 1 && j > 0 && waterGrid.get(i + 1, j - 1))
					|| (j > 0 && waterGrid.get(i, j - 1)) || (j < m_MapSize - 1 && waterGrid.get(i, j + 1))
					)
					shoreGrid.set(i, j, 0);
				else
					shoreGrid.set(i, j, shoreMax);
			}
			// If we want to expand on water, we want water tiles not to be shore tiles
			else if (expandOnWater)
				shoreGrid.set(i, j, shoreMax);
		}
	}

	// Expand influences on land to find shore distance
	for (u16 y = 0; y < m_MapSize; ++y)
	{
		u16 min = shoreMax;
		for (u16 x = 0; x < m_MapSize; ++x)
		{
			if (!waterGrid.get(x, y) || expandOnWater)
			{
				u16 g = shoreGrid.get(x, y);
				if (g > min)
					shoreGrid.set(x, y, min);
				else if (g < min)
					min = g;

				++min;
			}
		}
		for (u16 x = m_MapSize; x > 0; --x)
		{
			if (!waterGrid.get(x - 1, y) || expandOnWater)
			{
				u16 g = shoreGrid.get(x - 1, y);
				if (g > min)
					shoreGrid.set(x - 1, y, min);
				else if (g < min)
					min = g;

				++min;
			}
		}
	}
	for (u16 x = 0; x < m_MapSize; ++x)
	{
		u16 min = shoreMax;
		for (u16 y = 0; y < m_MapSize; ++y)
		{
			if (!waterGrid.get(x, y) || expandOnWater)
			{
				u16 g = shoreGrid.get(x, y);
				if (g > min)
					shoreGrid.set(x, y, min);
				else if (g < min)
					min = g;

				++min;
			}
		}
		for (u16 y = m_MapSize; y > 0; --y)
		{
			if (!waterGrid.get(x, y - 1) || expandOnWater)
			{
				u16 g = shoreGrid.get(x, y - 1);
				if (g > min)
					shoreGrid.set(x, y - 1, min);
				else if (g < min)
					min = g;

				++min;
			}
		}
	}

	return shoreGrid;
}

void MyPathFinder::ComputeShortPath(const IObstructionTestFilter& filter, entity_pos_t x0, entity_pos_t z0, entity_pos_t clearance,
	entity_pos_t range, const PathGoal& goal, pass_class_t passClass, WaypointPath& ret)
{
	ComputeShortPath2(m_cmpObstructionManager, filter, x0, z0, clearance, range, goal, passClass, ret);
}

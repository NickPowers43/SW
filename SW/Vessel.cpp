
#include "Vessel.h"

#include "Tile.h"
#include "TileChunk.h"
#include "Player.h"
#include "VesselObject.h"
#include "VesselModule.h"
#include "AdjacentTiles.h"


namespace SW
{
	Vessel::Vessel(VesselIndex_t index)
	{
		
		Vessel::index = index;
	}

	Vessel::~Vessel()
	{
	}

	int Vessel::ModulesOverlapping(AABBi aabb)
	{
		int output = 0;
		for (size_t i = 0; i < modules.size(); i++)
		{
			if (AABBi::OverlappingExclusive(modules[i].GetAABB(), aabb))
			{
				output++;
			}
		}
		return output;
	}
	int Vessel::ModulesOverlapping(glm::ivec2 point)
	{
		int output = 0;
		for (size_t i = 0; i < modules.size(); i++)
		{
			if (modules[i].GetAABB().ContainsExclusive(point - modules[i].pos))
			{
				output++;
			}
		}
		return output;
	}
	bool Vessel::IsModuleLegal(VesselModule* vm)
	{
		if (ModulesOverlapping(vm->GetAABB()))
		{
			return false;
		}

		return true;
	}
	bool Vessel::IsModuleLegal(VMType_t type, glm::ivec2 position)
	{
		return IsModuleLegal(vesselModuleTemplates[type], position);
	}
	bool Vessel::IsModuleLegal(VesselModuleTemplate* tLate, glm::ivec2 position)
	{
		for (size_t i = 0; i < modules.size(); i++)
		{
			if (true)
			{

				return false;
			}
		}

		return true;
	}

	void Vessel::AddModule(VesselModule vm)
	{
		modules.push_back(vm);


	}
	void Vessel::RemoveModuleAt(size_t index)
	{
		if (index < modules.size())
		{
			modules.erase(modules.begin() + index);
		}
	}
}
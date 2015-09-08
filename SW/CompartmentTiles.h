#pragma once
#include "TileSet.h"
#include "TrackedTileSet.h"

namespace SW
{
	class CompartmentTiles :
		virtual public TileSet,
		virtual public TrackedTileSet
	{
	public:
		CompartmentTiles();
		~CompartmentTiles();

		virtual Compartment* CreateCompartment();
		virtual void CopyCompartments(Tile* src, Tile* dst);
		virtual void SetC0(Tile* tile, Compartment* c);
		virtual void SetC1(Tile* tile, Compartment* c);
		static Compartment* GetC0(Tile* tile);
		static Compartment* GetC1(Tile* tile);

		void FillTile(AdjacentTiles* t);
		void RebuildCompartments();
		Compartment* CompartmentAt(glm::vec2 world);
		void SetCompartmentFloor(uint8_t type, Compartment* c);
	};
}


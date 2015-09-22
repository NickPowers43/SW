#pragma once
#include "TileSet.h"
#include "TrackedTileSet.h"

namespace SW
{
	class CompartmentTileSet :
		virtual public TileSet
	{
	public:
		CompartmentTileSet();
		~CompartmentTileSet();

		virtual Tile* CreateTile() override;
		virtual void DestroyTile(Tile* tile) override;

		virtual Compartment* CreateCompartment();
		virtual void CopyCompartments(Tile* src, glm::ivec2 location, Tile* dst);
		virtual float C0Area(Tile* tile, glm::ivec2 location);
		virtual float C0Area(AdjacentTiles* t);
		virtual void SetC0(AdjacentTiles* t, Compartment* c);
		virtual void SetC1(AdjacentTiles* t, Compartment* c);
		virtual void SetC0(Tile* tile, glm::ivec2 location, Compartment* c);
		virtual void SetC1(Tile* tile, glm::ivec2 location, Compartment* c);
		virtual void SetC0(Tile* tile, glm::ivec2 location, float area, Compartment* c);
		virtual void SetC1(Tile* tile, glm::ivec2 location, float area, Compartment* c);
		virtual void SetCompartmentFloor(FloorType_t type, Compartment* c);
		virtual void SetCompartmentFloorAt(FloorType_t type, glm::vec2 world);

		static Compartment* GetC0(Tile* tile);
		static Compartment* GetC1(Tile* tile);

		void FillTile(bool force, AdjacentTiles* t);
		void EraseCompartments(AABBi aabb);
		void RebuildCompartmentsForRegion(bool force, AABBi aabb);
		void RebuildCompartments(bool force);
		Compartment* CompartmentAt(glm::vec2 world);
	};
}


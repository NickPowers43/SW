#pragma once
#include "Tile.h"

namespace SW
{
	class CompartmentTile :
		public Tile
	{
	public:
		CompartmentTile();
		~CompartmentTile();

		Compartment* c0;
		Compartment* c1;
	};
}


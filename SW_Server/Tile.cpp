#include "stdafx.h"
#include "Tile.h"


namespace SW_Server
{
	Tile::Tile()
	{
	}


	Tile::~Tile()
	{
	}

	void Tile::WriteSetChunkMessage(SW::NetworkWriter* nw)
	{
		nw->Write(flags);
		nw->Write(wallMask);
		nw->Write(floor0);
		nw->Write(floor1);

		/*if (c0)
		{
			nw->Write(c0->index);
		}
		else
		{
			nw->Write(((1 << 33) - 1));
		}

		if (c1)
		{
			nw->Write(c1->index);
		}
		else
		{
			nw->Write(((1 << 33) - 1));
		}*/
	}
}

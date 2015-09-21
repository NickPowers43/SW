using UnityEngine;
using UnityEngine.Networking;
using System;
using System.Collections;
using System.Collections.Generic;
using Utility;

public class ServerVC : VesselChunk
{
	private bool seen;
	public bool Seen
	{
		get {
			return seen;
		}
		set {
			seen = value;
		}
	}
	public bool Modified
	{
		get{
			return updateMessageBytes;
		}
		set {
			updateMessageBytes = value;
		}
	}
	private bool updateMessageBytes;
	private byte[] messageBytes;
	public void WriteSetChunk(NetworkWriter nw)
	{
		if (updateMessageBytes) {

			NetworkWriter temp = new NetworkWriter();

			temp.Write(index.x);
			temp.Write(index.y);
			
			temp.Write(version);
			temp.Write(tileCount);
			
			for (int i = 0; i < VesselChunk.SIZE; i++) {
				
				for (int j = 0; j < VesselChunk.SIZE; j++) {
					
					VesselTile tile = TileAt(i, j);
					
					if (tile != null) {
						
						temp.Write(i);
						temp.Write(j);
						temp.Write((byte)tile.floor0);
						temp.Write((byte)tile.floor1);
						temp.Write((byte)tile.wallMask);
						temp.Write(tile.wallNode);
						
					}
				}
			}
			
			messageBytes = temp.ToArray();
			
			updateMessageBytes = false;
		}

		nw.WriteBytesFull(messageBytes);
	}
	
	public ServerVC (Vec2i Index, uint version) : base(Index, version)
	{
		updateMessageBytes = true;
	}

	public ServerVC (Vec2i Index) : base(Index, 0)
	{
		updateMessageBytes = true;
	}
}


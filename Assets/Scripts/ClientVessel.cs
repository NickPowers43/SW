using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class ClientVessel : Vessel
{
	public ClientVessel (uint index, NetworkReader nr) :
		base(index)
	{
		Sync(nr);
	}

	public void Sync(NetworkReader nr)
	{
		this.interiorPosition = nr.ReadVector2();
	}

	public void AddOurselves(ClientPlayer player, NetworkWriter nw)
	{
		player.currentVessel = this;
		player.ChunkI = WorldToChunkI((Vector2)player.transform.position);

		int rangeT = (PLAYER_CHUNK_RANGE * 2) + 1;

		nw.Write((ushort)ClientMessageType.RequestChunk);
		nw.Write((ushort)(rangeT * rangeT));

		for (int i = 0; i < rangeT; i++) {
			for (int j = 0; j < rangeT; j++) {
				
				Vec2i temp = player.ChunkI + new Vec2i(i - PLAYER_CHUNK_RANGE, j - PLAYER_CHUNK_RANGE);
				if (!((temp - player.ChunkI) <= PLAYER_CHUNK_RANGE)) {
					VesselChunk temp2 = chunks.TryGet(temp.x, temp.y);
					
					nw.Write(temp.x);
					nw.Write(temp.y);

					if (temp2 != null) {
						nw.Write(temp2.version);
					} else {
						nw.Write((uint)uint.MaxValue);
					}
				}
			}
		}
	}

	public void ReadSetChunkMessage(NetworkReader nr)
	{
		ClientVC chunk = new ClientVC(nr);

		VesselChunk current = chunks.TryGet(chunk.Index);

		if (current != null) {
			current.Destroy();
		}

		Debug.Log ("Setting chunk (" + chunk.Index.x + ", " + chunk.Index.y + ")");
		chunks.Set(chunk.Index, chunk);
		InstantiateChunk(chunk);
	}

	public void ReinstantiateChunks()
	{
		for (int i = 0; i < chunks.dim.y; i++) {
			for (int j = 0; j < chunks.dim.x; j++) {
				Vec2i chunkI = new Vec2i(j, i) + chunks.origin;
				ClientVC chunk;
				if ((chunk = (ClientVC)chunks.TryGet(chunkI)) != null) {
					if (chunk.Modified) {
						foreach (var c in chunks.WithinRange(chunk.Index, 1)) {
							if (c != null) {
								if (c.Instantiated) {
									c.Destroy();
								}
								Debug.Log ("Instantiating chunk: " + c.Index.ToString());
								InstantiateChunk(c);
							}
						}
						chunk.Modified = false;
					}
				}
			}
		}
	}
}


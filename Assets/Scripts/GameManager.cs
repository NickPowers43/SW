
using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Collections.Generic;

public enum ServerMessageType : ushort {
	SetChunk,
	SyncVessel,
	UpdatePlayer,
	RemovePlayer,
	AddPlayer
}

public enum ClientMessageType : ushort {
	RequestChunk,
	Inputs,
	FillAt
}

public class GameManager : NetworkBehaviour {

	public GameObject playerPrefab;

	private bool _isStarted = false;
	private bool _isServer = false;
	string defaultIP  = "160.39.38.100";
	int defaultPort = 7777;
	
	private int m_ConnectionId = 0;
	private int m_WebSocketHostId = 0;
	private int m_GenericHostId = 0;
	
	private ConnectionConfig m_Config = null;
	private static byte channelId = 0;
	public static byte ChannelId
	{
		get {
			return channelId;
		}
		set {
			channelId = value;
		}
	}
	private int clientConnectionId = 0;
	private HostTopology topology;

	public ClientVessel currentVessel = null;
	public ClientPlayer currentPlayer = null;
	public Dictionary<uint, ClientVessel> clientVessels = new Dictionary<uint, ClientVessel>();

#if !(UNITY_WEBGL && !UNITY_EDITOR)
	//Server fields
	private Dictionary<int, ServerPlayer> serverPlayers = new Dictionary<int, ServerPlayer>();
	private List<ServerVessel> serverVessels = new List<ServerVessel>();
#endif

	void Start()
	{
		m_Config = new ConnectionConfig();
		ChannelId = m_Config.AddChannel(QosType.Reliable);
		topology = new HostTopology(m_Config, 2048);
		
		GlobalVariables.CharacterInterior = LayerMask.NameToLayer("CharacterInterior");
		GlobalVariables.CharacterExterior = LayerMask.NameToLayer("CharacterExterior");
	}

	public void OnGUI()
	{
		const int PADDING = 5;
		const int BOX_WIDTH = 150;
		const int BOX_HEIGHT = 25;

		if (!_isStarted) {
			
			string ip = GUI.TextField(new Rect(PADDING, BOX_HEIGHT+(PADDING*2), BOX_WIDTH, BOX_HEIGHT), defaultIP);
			string port = GUI.TextField(new Rect(BOX_WIDTH+(PADDING*2), BOX_HEIGHT+(PADDING*2), BOX_WIDTH, BOX_HEIGHT), defaultPort.ToString());

#if !(UNITY_WEBGL && !UNITY_EDITOR)
			if (GUI.Button(new Rect(BOX_WIDTH+(PADDING*2), PADDING, BOX_WIDTH, BOX_HEIGHT), "Host"))
			{
				Debug.Log("Hosting at (0.0.0.0:" + port + ")");
				_isStarted = true;
				_isServer = true;
				NetworkTransport.Init();
				Application.targetFrameRate = 20;
				
				//add 2 host one for udp another for websocket, as websocket works via tcp we can do this
				m_WebSocketHostId = NetworkTransport.AddWebsocketHost(topology, int.Parse(port), null);
				//m_GenericHostId = NetworkTransport.AddHost(topology, int.Parse(port), null);
			}
#endif
			if (GUI.Button(new Rect(PADDING, PADDING, BOX_WIDTH, BOX_HEIGHT), "Connect"))
			{
				Debug.Log("Connecting to (" + ip + ":" + port + ")");
				_isStarted = true;
				_isServer = false;
				NetworkTransport.Init();

				//any port for udp client, for websocket second parameter is ignored, as webgl based game can be client only
				m_GenericHostId = NetworkTransport.AddHost(topology, int.Parse(port));
				byte error;
				m_ConnectionId = NetworkTransport.Connect(m_GenericHostId, ip, int.Parse(port), 0, out error);
			}
		} else {
			//GUI.Label(new Rect(BOX_WIDTH + PADDING, 20, 250, 500), "Min: " + minReceivedPing);
			//GUI.Label(new Rect(BOX_WIDTH + PADDING, 40, 250, 500), "Max: " + maxReceivedPing);
			//GUI.Label(new Rect(BOX_WIDTH + PADDING, 60, 250, 500), "Last: " + lastReceivedPing);

			if (GUI.Button(new Rect(PADDING, PADDING, BOX_WIDTH, BOX_HEIGHT), "Stop"))
			{
				Debug.Log("Shutting down");
				NetworkTransport.Shutdown();
#if !(UNITY_WEBGL && !UNITY_EDITOR)
				_isStarted = false;
#endif
			}
		}
	}
	
	private static GameManager instance = null;
	public static GameManager Instance
	{
		get{
			return instance;
		}
		set{
			instance = value;
		}
	}
	
	public GameObject wallNodePrefab;
	public GameObject wallSmallPrefab;
	public GameObject wallMediumPrefab;
	public GameObject wallLargePrefab;
	public GameObject chunkPrefab;

	public GameManager()
	{
		if (instance == null) {
			Instance = this;
		} else {
			//DestroyImmediate(this);
			return;
		}
	}

	void Update ()
	{
		if (!_isStarted)
			return;
		
		int recHostId;
		int connectionId;
		int channelId;
		byte[] recBuffer = new byte[1 << 14];
		int dataSize;
		byte error;

		if (!_isServer) {

			bool reinstantiateChunks = false;
			

			while (true) {

				NetworkEventType recData = NetworkTransport.Receive(out recHostId, out connectionId, out channelId, recBuffer, recBuffer.Length, out dataSize, out error);
				if(error != 0)
				{
					Debug.Log ((NetworkError)error + " " + dataSize + " ");
					return;
				}
				if (recData == NetworkEventType.Nothing) {
					break;
				}
				
				switch(recData) {
				case NetworkEventType.ConnectEvent:
				{
					Debug.Log("Successfully connected to server");
					clientConnectionId = connectionId;
					break;
				}
				case NetworkEventType.DataEvent:  //if server will receive echo it will send it back to client, when client will receive echo from serve wit will send other message
				{
					NetworkReader nr = new NetworkReader(recBuffer);
					
					while (true) {
						try {
							switch ((ServerMessageType)nr.ReadUInt16()) {
							case ServerMessageType.SetChunk:
								currentVessel.ReadSetChunkMessage(nr);
								reinstantiateChunks = true;
								break;
							case ServerMessageType.SyncVessel:
								uint vesselIndex = nr.ReadUInt32();
								ClientVessel cv;
								if (!clientVessels.TryGetValue(vesselIndex, out cv)) {
									cv = new ClientVessel(vesselIndex, nr);
								} else {
									cv.Sync(nr);
								}

								currentVessel = cv;
								break;
							default:
								break;
							}
						} catch (System.Exception ex) {
							break;
						}
					}
					
					if (reinstantiateChunks) {
						currentVessel.ReinstantiateChunks();
					}
					break;
				}
				case NetworkEventType.DisconnectEvent:
				{
					Debug.Log("Disconnect from host");
					_isStarted = false;
					break;
				}
				default:
					break;
				}
			}
			
			NetworkWriter nw = new NetworkWriter();

			if (Input.GetMouseButtonDown(0)) {
				nw.Write((ushort)ClientMessageType.FillAt);
				nw.Write(MainCamera.CursorPosition);
			}

			currentPlayer.WriteInputMessage(nw);

			if (nw.Position != 0) {
				NetworkTransport.Send(recHostId, clientConnectionId, ChannelId, nw.AsArray(), nw.Position, out error);
			}     
			if(error != 0)
				Debug.Log ((NetworkError)error);
		}
#if !(UNITY_WEBGL && !UNITY_EDITOR)
		else {

			NetworkReader nr = new NetworkReader(recBuffer);
			NetworkWriter nw = new NetworkWriter(new byte[1 << 14]);

			foreach (var vessel in ServerVessel.Vessels) {
				vessel.Update(nw);
			}

			NetworkEventType recData;
			while ((recData = NetworkTransport.Receive(out recHostId, out connectionId, out channelId, recBuffer, recBuffer.Length, out dataSize, out error)) != NetworkEventType.Nothing) {
				if(error != 0)
				{
					Debug.Log ((NetworkError)error + " " + dataSize + " ");
					return;
				}
				
				switch(recData) {
				case NetworkEventType.ConnectEvent:
				{
					Debug.Log("Client connecting");
					//start this player off from the beginning
					StartingVessel startVessel = StartingVessel.GetStartingVessel();
					GameObject playerGO = GameObject.Instantiate(playerPrefab) as GameObject;
					ServerPlayer player = playerGO.AddComponent<ServerPlayer>();
					serverPlayers.Add(recHostId, player);
					player.hostId = recHostId;
					startVessel.AddPlayer(player, nw);

					break;
				}
				case NetworkEventType.DataEvent:  //if server will receive echo it will send it back to client, when client will receive echo from serve wit will send other message
				{
					ServerPlayer player = serverPlayers[recHostId];

					nr.SeekZero();
					nw.SeekZero();
					while (nr.Position != dataSize) {
						switch ((ClientMessageType)nr.ReadUInt16()) {
						case ClientMessageType.RequestChunk:
							player.currentVessel.HandleChunkRequestMessage(nr, nw);
							break;
						case ClientMessageType.Inputs:
							player.ReadInputsMessage(nr);
							break;
						case ClientMessageType.FillAt:
							player.currentVessel.HandleFillAtMessage(nr);
							break;
						default:
							break;
						}
					}

					if (nw.Position != 0) {
						Debug.Log("Sending response: " + recHostId + "," + connectionId + "," + channelId + "," + nw.AsArray() + "," + nw.Position + ".");
						NetworkTransport.Send(recHostId, connectionId, channelId, nw.AsArray(), nw.Position, out error);
					}     
					if(error != 0)
						Debug.Log ((NetworkError)error);
					break;
				}
				case NetworkEventType.DisconnectEvent:
				{
					Debug.Log("Client disconnecting");
					ServerPlayer player = serverPlayers[recHostId];
					player.currentVessel.RemovePlayer(player, nw);
					serverPlayers.Remove(recHostId);
					break;
				}
				default:
					break;
				}
			}
			if(error != 0)
			{
				Debug.Log ((NetworkError)error + " " + dataSize + " ");
				return;
			}
		}
#endif
	}
}

public class GlobalVariables
{
	private static int characterInterior;
	public static int CharacterInterior
	{
		get{
			return characterInterior;
		}
		set{
			characterInterior = value;
		}
	}
	private static int characterExterior;
	public static int CharacterExterior
	{
		get{
			return characterExterior;
		}
		set{
			characterExterior = value;
		}
	}
}
	
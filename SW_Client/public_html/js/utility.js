
var ServerMessageType = {
    SetChunk : 0,
    SyncVessel : 1,
    UpdatePlayer : 2,
    RemovePlayer : 3,
    AddPlayer : 4,
    AddYourself : 5,
    MakeVesselActive : 6,
    PingMessage : 7
};


var ClientMessageType = {
    RequestChunk : 0,
    Inputs : 1,
    FillAt : 2,
    PingMessageResponse : 3
};


var ObjectType = {
    Spawner : 0,
    Turret : 1
};

var WallType = {
    None : 0,
    OneByZero : 1,
    TwoByOne : 2,
    OneByOne : 3,
    OneByTwo : 4,
    ZeroByOne : 5,
    OneByTwoFlipped : 6,
    OneByOneFlipped : 7,
    TwoByOneFlipped : 8
};

var WallTypeMask = {
    None : 0,
    OneByZero : 1,
    TwoByOne : 2,
    OneByOne : 4,
    OneByTwo : 8,
    ZeroByOne : 16,
    OneByTwoFlipped : 32,
    OneByOneFlipped : 64,
    TwoByOneFlipped : 128
};

var FloorType = {
    None : 0,
    Basic : 1,
    SmoothWhite : 2
};

//Physics
//var physicsWorld = new b2World();

//Network
var littleEndian = true;
var server_address;
var socket = null;
var socket_started = false;
var messages_out = [];
var messages_in = [];

//WebGL
THREE.ImageUtils.crossOrigin = 'anonymous';
var floorTexResolution = 256.0;
var renderer = new THREE.WebGLRenderer();
renderer.setSize( 800, 600 );
document.body.appendChild( renderer.domElement );
var scene = new THREE.Scene();
var camera = new THREE.PerspectiveCamera( 75, 800.0 / 600.0, 0.1, 1000 );
var floorMaterial = null;

//static floor and wall data
var PlayerChunkRange = 2;
var VesselChunkSize = 8;
var VesselChunkSizeF = parseFloat(VesselChunkSize);
var VesselChunkDataLength = VesselChunkSize * VesselChunkSize;
var wallOffsets = [
    new Vec2i(0,0),
    new Vec2i(1,0),
    new Vec2i(2,1),
    new Vec2i(1,1),
    new Vec2i(1,2),
    new Vec2i(0,1),
    new Vec2i(-1,2),
    new Vec2i(-1,1),
    new Vec2i(-2,1)
];
//dim0 - floor0 type
//dim1 - floor1 type
//dim2 - wall type
//dim3 - offset along wall
var floorMeshes = [];
var floorUVRects = [];
floorUVRects.push(new Rect(0.0, 0.0, 32.0, 32.0));
floorUVRects.push(new Rect(32.0, 0.0, 32.0, 32.0));

//Game data
var connected = false;
var currentVessel = null;
var myPlayer = null;
var players = [];
var vessels = [];
var instantiatedChunks = [];

//Constructors
function Vec2i(x, y) {
    this.x = x;
    this.y = y;
}

function AABBi(bl, tr) {
    this.bl = bl;
    this.tr = tr;
}

function DynamicArray2D() {
    this.origin = new Vec2i(0,0);
    this.dim = new Vec2i(0,0);
    this.data = [];
}

function Vessel(index) {
    this.index = index;
    this.chunks = new DynamicArray2D();
    this.data = [];
    for(var i = 0; i < VesselChunkDataLength; i++) {
        this.data.push(null);
    }
}

function VesselTile(flags, wallMask, c0, c1, floor0, floor1) {
    this.flags = flags;
    this.wallMask = wallMask;
    this.c0 = c0;
    this.c1 = c1;
    this.floor0 = floor0;
    this.floor1 = floor1;
}

function VesselChunk(index, version) {
    this.modified = true;
    this.seen = false;
    this.index = index;
    this.version = version;
    this.data = [];
    this.mesh = null;
}

function Player(pos) {
    this.pos = pos;
}

function Rect(x, y, width, height) {
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
}

function FloorMesh(v, i) {
    this.v = v;
    this.i = i;
}

function PosUVPair(pos, uv) {
    this.pos = pos;
    this.uv = uv;
}





var AddVec2i = function(a, b) {
    return new Vec2i(a.x + b.x, a.y + b.y);
};
var SubVec2i = function(a, b) {
    return new Vec2i(a.x - b.x, a.y - b.y);
};

var WorldToChunkI = function(world) {
    return new Vec2i(
        (world.x < 0.0) ? -1 - parseInt(world.x / -VesselChunkSizeF) : parseInt(world.x / VesselChunkSizeF),
        (world.y < 0.0) ? -1 - parseInt(world.y / -VesselChunkSizeF) : parseInt(world.y / VesselChunkSizeF));
};

var TileToChunkI = function(tileI) {
    return new Vec2i(
        (tileI.x >= 0) ? tileI.x / VesselChunkSize : ((tileI.x + 1) / VesselChunkSize) - 1,
        (tileI.y >= 0) ? tileI.y / VesselChunkSize : ((tileI.y + 1) / VesselChunkSize) - 1);
};

var MatToLinear = function(x, y, columns) {
    return (y * columns) + x;
};

//static Networking functions

var Connect = function(address)
{
    server_address = address;
    socket = new WebSocket('ws://' + address);
    socket.binaryType = 'arraybuffer';

    // Open the socket
    socket.onopen = function(event) {
        
        console.log('Connection established with server at: ' + server_address);
        connected = true;

        socket_started = true;

        while (messages_out.length > 0){
            socket.send(messages_out.shift());
        }

        // Listen for messages
        socket.onmessage = function(event) {
            var data = event.data;
            messages_in.push(new DataView(data));
        };

        // Listen for socket closes
        socket.onclose = function(event) {
            console.log('Client notified socket has closed',event);
            socket = null;
            Close();
        };
    };
};
var Send = function(data)
{
    if(socket_started)
    {
        socket.send(data);
    }
    else
    {
        messages_out.push(data);
    }
};
var Close = function()
{
    //Reset networking data
    if (socket != null) {
        socket.close();
    }
    socket = null;
    socket_started = false;
    messages_out = [];
    message_in = [];
    connected = false;
    
    //Reset game data
    vessels = [];
};
var Receive = function()
{
    if(messages_in.length > 0){
        return messages_in.shift();
    } else {
        return null;
    }
};

var AppendMeshData = function(floorMesh, vertices, uv, indices, offset) {
    var startI = vertices.length;
    for (var i = 0; i < floorMesh.i.length; i++) {
        indices.push(floorMesh.i[i] + startI);
    }


    for (var i = 0; i < floorMesh.v.length; i++) {
        var temp = floorMesh.v[i].pos;
        vertices.push(new THREE.Vector3(temp.x + offset.x, temp.y + offset.y, temp.z + offset.z));
        uv.push(floorMesh.v[i].uv);
    }
};

var AddTriangle = function(i) {
    var start = i.length;
    i.push(start + 0);
    i.push(start + 2);
    i.push(start + 1);
};

var AddQuad = function(i) {
    var start = i.length;
    i.push(start + 0);
    i.push(start + 2);
    i.push(start + 1);
    i.push(start + 1);
    i.push(start + 2);
    i.push(start + 3);
};

var GenerateBaseVertices = function(a) {
    
    var output = [];
    
    //0
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,0.0,0.0),
        new THREE.Vector2(a.x, a.y)));
    //1
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,1.0,0.0),
        new THREE.Vector2(a.x, a.y + a.height)));
    //2
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,0.0,0.0),
        new THREE.Vector2(a.x + a.width, a.y)));
    //3
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,1.0,0.0),
        new THREE.Vector2(a.x + a.width, a.y + a.height)));
    
    //4
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,0.5,0.0),
        new THREE.Vector2(a.x, a.y + (a.height * 0.5))));
    //5
    output.push(new PosUVPair(
        new THREE.Vector3(0.5,1.0,0.0),
        new THREE.Vector2(a.x + (a.width * 0.5), a.y + a.height)));
    //6
    output.push(new PosUVPair(
        new THREE.Vector3(0.5,0.0,0.0),
        new THREE.Vector2(a.x + (a.width * 0.5), a.y)));
    //7
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,0.5,0.0),
        new THREE.Vector2(a.x + a.width, a.y + (a.height * 0.5))));
        
    return output;
};

var GenerateMeshes = function(type, rightFloor, none0, leftFloor, none1) {
    
    if (none0 && none1) {
        return null;
    }
    
    var output = [];
    
    var v0 = GenerateBaseVertices(rightFloor);
    var v1 = GenerateBaseVertices(leftFloor);
    
    var iTemp = [];
    var vTemp = [];
    
    if (type == WallType.None) {
        //wall type None (simple quad)
        if (!none0) {
            vTemp.push(v0[0]);
            vTemp.push(v0[1]);
            vTemp.push(v0[2]);
            vTemp.push(v0[3]);
            AddQuad(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.TwoByOne) {
        if (!none0) {
            vTemp.push(v0[0]);
            vTemp.push(v0[7]);
            vTemp.push(v0[2]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[7]);
            vTemp.push(v1[3]);
            AddQuad(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
        vTemp = [];
        iTemp = [];
        if (!none0) {
            vTemp.push(v0[0]);
            vTemp.push(v0[4]);
            vTemp.push(v0[2]);
            vTemp.push(v0[3]);
            AddQuad(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[4]);
            vTemp.push(v1[1]);
            vTemp.push(v1[3]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.OneByOne) {
        if (!none0) {
            vTemp.push(v0[0]);
            vTemp.push(v0[3]);
            vTemp.push(v0[2]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[3]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.OneByTwo) {
        if (!none0) {
            vTemp.push(v0[0]);
            vTemp.push(v0[5]);
            vTemp.push(v0[2]);
            vTemp.push(v0[3]);
            AddQuad(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[5]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
        vTemp = [];
        iTemp = [];
        if (!none0) {
            vTemp.push(v0[6]);
            vTemp.push(v0[3]);
            vTemp.push(v0[2]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[6]);
            vTemp.push(v1[3]);
            AddQuad(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.OneByTwoFlipped) {
        if (!none0) {
            vTemp.push(v0[2]);
            vTemp.push(v0[5]);
            vTemp.push(v0[3]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[2]);
            vTemp.push(v1[5]);
            AddQuad(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
        vTemp = [];
        iTemp = [];
        if (!none0) {
            vTemp.push(v0[6]);
            vTemp.push(v0[1]);
            vTemp.push(v0[2]);
            vTemp.push(v0[3]);
            AddQuad(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[6]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.OneByOneFlipped) {
        //wall type None (simple quad)
        if (!none0) {
            vTemp.push(v0[2]);
            vTemp.push(v0[1]);
            vTemp.push(v0[3]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[0]);
            vTemp.push(v1[1]);
            vTemp.push(v1[2]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else if (type == WallType.TwoByOneFlipped) {
        if (!none0) {
            vTemp.push(v0[2]);
            vTemp.push(v0[4]);
            vTemp.push(v0[3]);
            vTemp.push(v0[1]);
            AddQuad(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[2]);
            vTemp.push(v1[0]);
            vTemp.push(v1[4]);
            AddTriangle(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
        vTemp = [];
        iTemp = [];
        if (!none0) {
            vTemp.push(v0[7]);
            vTemp.push(v0[1]);
            vTemp.push(v0[3]);
            AddTriangle(iTemp);
        }
        if (!none1) {
            vTemp.push(v1[2]);
            vTemp.push(v1[0]);
            vTemp.push(v1[7]);
            vTemp.push(v1[1]);
            AddQuad(iTemp);
        }
        output.push(new FloorMesh(vTemp, iTemp));
    } else {
        return null;
    }
    return output;
};

var GenerateFloorMeshes = function() {
    for (var i = 0; i < floorUVRects.length; i++) {
        var temp = [];
        for (var j = 0; j < floorUVRects.length; j++) {
            temp.push([]);
        }
        floorMeshes.push(temp);
    }
    
    
    for (var i = 0; i < floorUVRects.length; i++) {
        for (var j = 0; j < floorUVRects.length; j++) {
            for (var k = 0; k < 9; k++) {
                floorMeshes[i][j].push(GenerateMeshes(
                    k,
                    (i == 0) ? floorUVRects[i] : floorUVRects[i-1],
                    i == 0,
                    (j == 0) ? floorUVRects[j] : floorUVRects[j-1],
                    j == 0));
            }
        }
    }
};

var InitializeMeshData = function() {
    
    var scale = 1.0 / floorTexResolution;
    
    for(var i = 0; i < floorUVRects.length; i++) {
        var rect = floorUVRects[i];
        rect.x *= scale;
        rect.y *= scale;
        rect.width *= scale;
        rect.height *= scale;
    }
    
    GenerateFloorMeshes();
};

var InitializeGame = function() {
    
    InitializeMeshData();
    
    var texture = THREE.ImageUtils.loadTexture('img/floors.png');
    floorMaterial = new THREE.MeshBasicMaterial({map: texture, color: 0xffffff});
    
};

//VesselChunk method definitions

VesselChunk.prototype.SetTile = function(offset, val) {
    this.data[offset.x + (offset.y * VesselChunkSize)] = val;
};

VesselChunk.prototype.TileAt = function(offset) {
    return this.data[offset.x + (offset.y * VesselChunkSize)];
};

VesselChunk.prototype.TileAt = function(x, y) {
    return this.data[x + (y * VesselChunkSize)];
};

VesselChunk.prototype.ChunkIToWorld = function() {
    return new THREE.Vector2(
        this.index.x * VesselChunkSizeF,
        this.index.y * VesselChunkSizeF);
};

VesselChunk.prototype.Destroy = function() {
    if (this.mesh != null) {
        scene.remove(this.mesh)
        this.mesh = null;
    }
};

VesselChunk.prototype.GenerateFloorMesh = function(t, l, r, b, br) {
    var vertices = [];
    var uv = [];
    var indices = [];

    for (var i = 0; i < VesselChunkSize; i++) {
        for (var j = 0; j < VesselChunkSize; j++) {

            var tile = this.TileAt(i, j);

            if (tile != null) {

                if (tile.floor0 != FloorType.None || tile.floor1 != FloorType.None) {

                    var offset = new THREE.Vector3(i, j, 0.0);

                    //VesselTile tTile = (j === VesselChunkSize-1) ? (t != null) ? t.TileAt(i,0) : null : TileAt(i,j+1);
                    var lTile = (i == 0) ? (l != null) ? l.TileAt(VesselChunkSize-1,j) : null : this.TileAt(i-1,j);
                    var rTile = (i == VesselChunkSize-1) ? (r != null) ? r.TileAt(0,j) : null : this.TileAt(i+1,j);
                    var r2Tile = (i >= VesselChunkSize-2) ? (r != null) ? r.TileAt(i-(VesselChunkSize-2),j) : null : this.TileAt(i+2,j);
                    var bTile = (j == 0) ? (b != null) ? b.TileAt(i,VesselChunkSize-1) : null : this.TileAt(i,j-1);
                    var brTile = null;
                    if (j == 0) {
                        if (i < VesselChunkSize-1) {
                            brTile = (b != null) ? b.TileAt(i+1,VesselChunkSize-1) : null;
                        } else {
                            brTile = (br != null) ? br.TileAt(0,VesselChunkSize-1) : null;
                        }
                    } else {
                        if (i != VesselChunkSize-1) {
                            brTile = this.TileAt(i+1,j-1);
                        } else {
                            brTile = (r != null) ? r.TileAt(0,j-1) : null;
                        }
                    }
                    
                    if (tile.floor0 == tile.floor1) {
                        AppendMeshData(floorMeshes[tile.floor0][FloorType.None][WallType.None][0],vertices,uv,indices,offset);
                    } else {
                        var floorCombMeshes = floorMeshes[tile.floor0][tile.floor1];

                        if (tile.ContainsWallMask(WallTypeMask.TwoByOne)) { //this tile contains a TwoByOne
                            AppendMeshData(floorCombMeshes[WallType.TwoByOne][0],vertices,uv,indices,offset);
                        } else if (tile.ContainsWallMask(WallTypeMask.OneByTwo)) { //this tile contains a OneByTwo
                            AppendMeshData(floorCombMeshes[WallType.OneByTwo][0],vertices,uv,indices,offset);
                        } else if (tile.ContainsWallMask(WallTypeMask.OneByOne)) { //this tile contains a OneByOne
                            AppendMeshData(floorCombMeshes[WallType.OneByOne][0],vertices,uv,indices,offset);
                        } 
                        else if (lTile != null && lTile.ContainsWallMask(WallTypeMask.TwoByOne)) { //left tile contains a TwoByOne
                            AppendMeshData(floorCombMeshes[WallType.TwoByOne][1],vertices,uv,indices,offset);
                        } else if (bTile != null && bTile.ContainsWallMask(WallTypeMask.OneByTwo)) { //bottom tile contains a OneByTwo
                            AppendMeshData(floorCombMeshes[WallType.OneByTwo][1],vertices,uv,indices,offset);
                        } else if (brTile != null && brTile.ContainsWallMask(WallTypeMask.OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
                            AppendMeshData(floorCombMeshes[WallType.OneByTwoFlipped][1],vertices,uv,indices,offset);
                        } else if (rTile != null && rTile.ContainsWallMask(WallTypeMask.TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
                            AppendMeshData(floorCombMeshes[WallType.TwoByOneFlipped][0],vertices,uv,indices,offset);
                        } else if (rTile != null && rTile.ContainsWallMask(WallTypeMask.OneByOneFlipped)) { //r tile contains a OneByOneFlipped
                            AppendMeshData(floorCombMeshes[WallType.OneByOneFlipped][0],vertices,uv,indices,offset);
                        } else if (rTile != null && rTile.ContainsWallMask(WallTypeMask.OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
                            AppendMeshData(floorCombMeshes[WallType.OneByTwoFlipped][0],vertices,uv,indices,offset);
                        } else if (r2Tile != null && r2Tile.ContainsWallMask(WallTypeMask.TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
                            AppendMeshData(floorCombMeshes[WallType.TwoByOneFlipped][1],vertices,uv,indices,offset);
                        } else {
                            //no walls cut this tile
                            AppendMeshData(floorMeshes[tile.floor0][FloorType.None][WallType.None][0],vertices,uv,indices,offset);
                        }
                    }
                }
            }
        }
    }

    var output = new THREE.Geometry();

    output.vertices = vertices;
    var faces = [];
    var faceUVs = [];
    while(indices.length > 0) {
        var a = indices.shift();
        var b = indices.shift();
        var c = indices.shift();
        faces.push(new THREE.Face3(a, b, c));
        //var uvA = uv.shift();
        //var uvB = uv.shift();
        //var uvC = uv.shift();
        faceUVs.push([uv[a], uv[b], uv[c]]);
    }
    output.faceVertexUvs[0] = faceUVs;
    output.faces = faces;
    
    output.verticesNeedUpdate = true;
    output.uvsNeedUpdate = true;
    output.elementsNeedUpdate = true;

    output.computeBoundingSphere();
    
    return output;
};

VesselChunk.prototype.Instantiate = function(t, l, r, b, br) {
    
    this.Destroy();
    
    var chunkOffset = this.ChunkIToWorld();
    var floorMesh = this.GenerateFloorMesh(t,l,r,b,br);
    
    this.mesh = new THREE.Mesh(floorMesh, floorMaterial);
    this.mesh.position = new THREE.Vector3(chunkOffset.x, chunkOffset.y, 0.0);
    scene.add(this.mesh);
};

VesselChunk.prototype.OriginTileIndex = function() {
    return new Vec2i(
        this.index.x * VesselChunkSize,
        this.index.y * VesselChunkSize);
};

//Vessel method definitions

VesselTile.prototype.ContainsWall = function(wall) {
    return (this.wallMask & (1 << (wall - 1))) > 0;
};

VesselTile.prototype.ContainsWallMask = function(wall) {
    return (this.wallMask & wall) > 0;
};

Vessel.prototype.IsWallLegal = function(index, type) {
    //check if walls nodes are not too close
    //to the new wall
    if (!(type === 1 || type === 5)) {

        var hDir = (type < 5) ? 1 : -1;
        var diff = Math.abs(type - 5);

        if (hDir < 0) {
            if (this.ContainsWall(new Vec2i(index.x+hDir,index.y))) {
                return false;
            }
            if (diff != 2) {
                if (this.ContainsWall(new Vec2i(index.x-1,index.y+1))) {
                    return false;
                }
                if (diff === 3) {
                    if (this.ContainsWall(new Vec2i(index.x-2,index.y))) {
                        return false;
                    }
                } else if (diff === 1) {
                    if (this.ContainsWall(new Vec2i(index.x,index.y+2))) {
                        return false;
                    }
                }
            }
        } else {
            if (this.ContainsWall(new Vec2i(index.x,index.y+1))) {
                return false;
            }
            if (diff != 2) {
                if (this.ContainsWall(new Vec2i(index.x+1,index.y+1))) {
                    return false;
                }
                if (diff === 3) {
                    if (this.ContainsWall(new Vec2i(index.x+2,index.y))) {
                        return false;
                    }
                } else if (diff === 1) {
                    if (this.ContainsWall(new Vec2i(index.x,index.y+2))) {
                        return false;
                    }
                }
            }
        }
    }

    var end = AddVec2i(index, wallOffsets[type]);

    if (this.TooCloseToWall(end) || this.TooCloseToWall(index)) {
        return false;
    }

    return this.LegalWallStart(type, index) && this.LegalWallEnd(type, end);
};

Vessel.prototype.ContainsWall = function(index) {
    var tile = this.TryGetTile(index);
		
    if (tile != null) {
        if (tile.wallNode) {
            return true;
        } else {
            return false;
        }
    }

    return false;
};

Vessel.prototype.LegalWallStart = function(type, index) {
    
    var tile = TryGetTile(index);
    if (tile != null) {
        var walls = tile.GetWalls();

        if (walls.count > 1) {
            return false;
        }
        if (walls.count === 1) {
            if (Math.abs(walls.wall0 - type) < 4) {
                return false;
            }
        }
    }

    for (var i = 1; i < 9; i++) {
        tile = TryGetTile(SubVec2i(index, wallOffsets[i]));
        if (tile != null) {
            if (tile.ContainsWall(i)) {
                if (!this.NonAcuteSequence(i, type)) {
                    return false;
                }
            }
        }
    }

    return true;
};

Vessel.prototype.LegalWallEnd = function(type, index) {
    
    var tile = TryGetTile(index);
    if (tile != null) {
        var walls = tile.GetWalls();

        if (walls.count > 0) {
            if (!this.NonAcuteSequence(type, walls.wall0)) {
                return false;
            }
            if (walls.count > 1) {
                if (!this.NonAcuteSequence(type, walls.wall1)) {
                    return false;
                }
            }
        }
    }

    for (var i = 1; i < 9; i++) {
        tile = TryGetTile(SubVec2i(index, wallOffsets[i]));
        if (tile != null) {
            if (tile.ContainsWall(i)) {
                if (Math.abs(type - i) < 4) {
                    return false;
                }
            }
        }
    }

    return true;
};

Vessel.prototype.TooCloseToWall = function(index) {
    var tile;

    //0
    tile = TryGetTile(new Vec2i(index.x-1, index.y));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.OneByOne | WallTypeMask.TwoByOne | WallTypeMask.OneByTwo)) {
            return true;
        }
    }
    //1
    tile = TryGetTile(new Vec2i(index.x, index.y-1));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.OneByOne | WallTypeMask.OneByTwo | WallTypeMask.OneByOneFlipped | WallTypeMask.OneByTwoFlipped | WallTypeMask.TwoByOne | WallTypeMask.TwoByOneFlipped)) {
            return true;
        }
    }
    //WallTypeMask.TwoByOne
    tile = TryGetTile(new Vec2i(index.x+1, index.y));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.OneByOneFlipped | WallTypeMask.TwoByOneFlipped | WallTypeMask.OneByTwoFlipped)) {
            return true;
        }
    }
    //3
    tile = TryGetTile(new Vec2i(index.x-1, index.y-1));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.OneByTwo | WallTypeMask.TwoByOne)) {
            return true;
        }
    }
    //4
    tile = TryGetTile(new Vec2i(index.x+1, index.y-1));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.TwoByOneFlipped | WallTypeMask.OneByTwoFlipped)) {
            return true;
        }
    }
    //5
    tile = TryGetTile(new Vec2i(index.x-WallTypeMask.TwoByOne, index.y));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.TwoByOne)) {
            return true;
        }
    }
    //6
    tile = TryGetTile(new Vec2i(index.x+WallTypeMask.TwoByOne, index.y));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.TwoByOneFlipped)) {
            return true;
        }
    }
    //7
    tile = TryGetTile(new Vec2i(index.x, index.y-WallTypeMask.TwoByOne));
    if (tile != null) {
        if (tile.ContainsWallMask(WallTypeMask.OneByTwoFlipped | WallTypeMask.OneByTwo)) {
            return true;
        }
    }

    return false;
};

Vessel.prototype.NonAcuteSequence = function(wall0, wall1) {
    if (wall1 < WallType.ZeroByOne) {
        if (wall0 > (4 + wall1)) {
            return false;
        }
    } else if (wall1 > WallType.ZeroByOne) {
        if (wall0 < (wall1 - 4)) {
            return false;
        }
    }
};

Vessel.prototype.TryGetTile = function(index) {
    var chunkI = this.TileToChunkI(index);
    var vc = this.chunks.TryGet(chunkI);

    if (vc === null) {
        return null;
    } else {
        index = SubVec2i(index, vc.OriginTileIndex());
        return vc.TileAt(index);
    }
};


Vessel.prototype.Top = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x, chunk.index.y+1)); };
Vessel.prototype.Bottom = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x, chunk.index.y-1)); };
Vessel.prototype.Left = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x-1, chunk.index.y)); };
Vessel.prototype.Right = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x+1, chunk.index.y)); };
Vessel.prototype.TopLeft = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x-1, chunk.index.y+1)); };
Vessel.prototype.TopRight = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x+1, chunk.index.y+1)); };
Vessel.prototype.BottomLeft = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x-1, chunk.index.y-1)); };
Vessel.prototype.BottomRight = function (chunk) { return this.chunks.TryGet(new Vec2i(chunk.index.x+1, chunk.index.y-1)); };

Vessel.prototype.InstantiateChunk = function(chunk) {
    chunk.Instantiate(
        this.Top(chunk), 
        this.Left(chunk), 
        this.Right(chunk), 
        this.Bottom(chunk), 
        this.BottomRight(chunk));
};



//DynamicArray2D method definitions

DynamicArray2D.prototype.GrowTopRight = function(amount) {
    
    var newDim = AddVec2i(this.dim, amount);
    var newData = [];
    for(var i = 0; i < (newDim.x * newDim.y);i++) {
        newData.push(null);
    }
    
    for(var i = 0; i < this.dim.y;i++) {
        for(var j = 0; j < this.dim.x;j++) {
            newData[MatToLinear(j, i, newDim.x)] = this.data[MatToLinear(j, i, this.dim.x)];
        }
    }
    
    this.data = newData;
    this.dim = newDim;
};

DynamicArray2D.prototype.GrowBottomLeft = function(amount) {
    
    var newDim = AddVec2i(this.dim, amount);
    var newData = [];
    for(var i = 0; i < (newDim.x * newDim.y);i++) {
        newData.push(null);
    }
    
    for(var i = amount.y; i < newDim.y;i++) {
        for(var j = amount.x; j < newDim.x;j++) {
            newData[MatToLinear(j, i, newDim.x)] = this.data[MatToLinear(j - amount.x, i - amount.y, this.dim.x)];
        }
    }
    
    this.origin = SubVec2i(this.origin, amount);
    this.data = newData;
    this.dim = newDim;
};

DynamicArray2D.prototype.Contains = function(index) {
    
    return ((index.x >= this.origin.x) && (index.x < (this.dim.x + this.origin.x))) && 
        ((index.y >= this.origin.y) && (index.y < (this.dim.y + this.origin.y)));
};

DynamicArray2D.prototype.Contains = function(x, y) {
    
    return ((x >= this.origin.x) && (x < (this.dim.x + this.origin.x))) && 
        ((y >= this.origin.y) && (y < (this.dim.y + this.origin.y)));
};

DynamicArray2D.prototype.Set = function(x, y, val) {
    
    if (x < this.origin.x) {
        if (y < this.origin.y) {
            //expand bottom left
            this.GrowBottomLeft(new Vec2i(this.origin.x - x, this.origin.y - y));
        } else if (y >= this.dim.y + this.origin.y) {
            //expand top left
            this.GrowTopRight(new Vec2i(0, y - (this.dim.y + this.origin.y) + 1));
            this.GrowBottomLeft(new Vec2i(this.origin.x - x, 0));
        } else {
            //expand left
            this.GrowBottomLeft(new Vec2i(this.origin.x - x, 0));
        }
    } else if (x >= this.dim.x + this.origin.x) {
        if (y < this.origin.y) {
            //expand bottom right
            this.GrowTopRight(new Vec2i(x - (this.dim.x + this.origin.x) + 1, 0));
            GrowBottomLeft(new Vec2i(0, this.origin.y - y));
        } else if (y >= this.dim.y + this.origin.y) {
            //expand top right
            this.GrowTopRight(new Vec2i(x - (this.dim.x + this.origin.x) + 1, y - (this.dim.y + this.origin.y) + 1));
        } else {
            //expand right
            this.GrowTopRight(new Vec2i(x - (this.dim.x + this.origin.x) + 1, 0));
        }
    } else {
        if (y < this.origin.y) {
            //expand bottom
            this.GrowBottomLeft(new Vec2i(0, this.origin.y - y));
        } else if (y >= this.dim.y + this.origin.y) {
            //expand top
            this.GrowTopRight(new Vec2i(0, y - (this.dim.y + this.origin.y) + 1));
        }
    }

    x -= this.origin.x;
    y -= this.origin.y;
    this.data[MatToLinear(x, y, this.dim.x)] = val;
};

DynamicArray2D.prototype.TryGet = function(x, y) {
    if (!this.Contains(x, y))
        return null;

    x -= this.origin.x;
    y -= this.origin.y;
    return data [MatToLinear(x, y, this.dim.x)];
};

DynamicArray2D.prototype.TryGet = function(index) {
    if (!this.Contains(index))
        return null;

    index.x -= this.origin.x;
    index.y -= this.origin.y;
    return data [MatToLinear(index.x, index.y, this.dim.x)];
};

//AABBi method definitions

AABBi.prototype.FitWhole = function(point) {
    if (point.x < this.bl.x){
        this.bl.x = point.x;
    }
    if (point.y < this.bl.y){
        this.bl.y = point.y;
    }

    if (point.x >= this.tr.x){
        this.tr.x = point.x + 1;
    }
    if (point.y >= this.tr.y){
        this.tr.y = point.y + 1;
    }
};

AABBi.prototype.Fit = function(point) {
    if (point.x < this.bl.x){
        this.bl.x = point.x;
    }
    if (point.y < this.bl.y){
        this.bl.y = point.y;
    }

    if (point.x > this.tr.x){
        this.tr.x = point.x;
    }
    if (point.y > this.tr.y){
        this.tr.y = point.y;
    }
};

AABBi.prototype.FitAABB = function(aabb) {
    this.bl.x = Math.min(aabb.bl.x, this.bl.x);
    this.bl.y = Math.min(aabb.bl.y, this.bl.y);

    this.tr.x = Math.max(aabb.tr.x, this.tr.x);
    this.tr.y = Math.max(aabb.tr.y, this.tr.y);
};

var ClearCurrentVessel = function() {
    //clear players properly
    players = [];
};

var UpdateChunks = function(force) {
    
    var rangeT = (PlayerChunkRange * 2) + 1;
    var orgChunkI = myPlayer.chunkI;
    myPlayer.chunkI = WorldToChunkI(myPlayer.pos);

    if (force || ((orgChunkI.x != myPlayer.chunkI.x) || (orgChunkI.y != myPlayer.chunkI.y))) {
        
        for (var i = 0; i < currentVessel.chunks.dim.x; i++) {
            for (var j = 0; j < currentVessel.chunks.dim.y; j++) {
                var chunkI = AddVec2i(currentVessel.chunks.dim, new Vec2i(i, j));
                var existingChunk = currentVessel.chunks.TryGet(chunkI.x, chunkI.y);
                if (existingChunk != null) {
                    existingChunk.seen = false;
                }
            }
        }
        
        var buffer = new DataView(new ArrayBuffer(1 << 14));
        var byteOffset = 0;
        buffer.setUint8(byteOffset, ClientMessageType.RequestChunk);
        byteOffset += 1;
        var requestLength = 0;
        var lengthOffset = byteOffset;
        byteOffset += 1;

        for (i = 0; i < rangeT; i++) {
            for (j = 0; j < rangeT; j++) {

                var diffCurr = new Vec2i(i - PlayerChunkRange, j - PlayerChunkRange);
                var temp = AddVec2i(myPlayer.chunkI, diffCurr);
                var diffOrg = SubVec2i(temp, orgChunkI);
                if (force || ((Math.abs(diffOrg.x) > PlayerChunkRange) || (Math.abs(diffOrg.y) > PlayerChunkRange))) {

                    requestLength++;

                    var temp2 = currentVessel.chunks.TryGet(temp.x, temp.y);

                    buffer.setInt16(byteOffset, temp.x, littleEndian);
                    byteOffset += 2;
                    buffer.setInt16(byteOffset, temp.y, littleEndian);
                    byteOffset += 2;

                    if (temp2 != null) {
                        temp2.seen = true;
                        buffer.setUint32(byteOffset, temp2.version, littleEndian);
                        byteOffset += 4;
                    } else {
                        buffer.setUint32(byteOffset, (1 << 33) - 1, littleEndian);
                        byteOffset += 4;
                    }
                }
            }
        }
        if (requestLength > 0) {
            
            console.log("Requesting " + requestLength + " chunks from server");
            
            buffer.setUint8(lengthOffset, requestLength);
            Send(buffer.buffer.slice(0, byteOffset));
        }

        //destroy chunks that are no longer visible
        for (var i = 0; i < currentVessel.chunks.dim.x; i++) {
            for (var j = 0; j < currentVessel.chunks.dim.y; j++) {
                var chunkI = AddVec2i(currentVessel.chunks.dim, new Vec2i(i, j));

                var existingChunk = currentVessel.chunks.TryGet(chunkI.x, chunkI.y);
                if (existingChunk != null) {
                    if (existingChunk.seen) {
                        existingChunk.Instantiate();
                    } else {
                        existingChunk.Destroy();
                    }
                }
            }
        }
    }
};

//Main Loop
InitializeGame();

camera.position.z = 20;

window.performance.mark('physics_update');

function render() {
    
    var elapsedTime = window.performance.measure('physics_update');
    window.performance.clearMarks();
    window.performance.mark('physics_update');
    if (connected) {
        
        var message = Receive();
        if (message != null) {
            var byteOffset = 0;
            while(byteOffset != message.byteLength) {
                var messageIndex = message.getUint8(byteOffset);
                byteOffset += 1;
                switch (messageIndex) {
                    case ServerMessageType.PingMessage:
                        var buffer = new DataView(new ArrayBuffer(1 << 14));
                        var RespByteOffset = 0;
                        buffer.setUint8(RespByteOffset, ClientMessageType.PingMessageResponse);
                        RespByteOffset += 1;
                        buffer.setUint32(RespByteOffset, message.getUint32(byteOffset, littleEndian), littleEndian);
                        byteOffset += 4;
                        RespByteOffset += 4;
                        Send(buffer.buffer.slice(0, RespByteOffset));
                        break;
                    case ServerMessageType.SetChunk://SetChunk
                        var chunkCount = message.getUint8(byteOffset);
                        byteOffset += 1;
                        
                        console.log("Received " + chunkCount + " chunks from server");
                        
                        var newChunks = [];
                        
                        for (var i = 0; i < chunkCount; i++) {
                            var x = message.getInt16(byteOffset, littleEndian);
                            byteOffset += 2;
                            var y = message.getInt16(byteOffset, littleEndian);
                            byteOffset += 2;
                            var version = message.getUint32(byteOffset, littleEndian);
                            byteOffset += 4;
                            
                            var chunk = new VesselChunk(new Vec2i(x, y), version);
                            
                            //console.log("Received chunk at (" + x + ", " + y + ")");
                            
                            var tile_count = message.getUint16(byteOffset, littleEndian);
                            byteOffset += 2;
                            for (var j = 0; j < tile_count; j++) {
                                var tileLinearI = message.getUint16(byteOffset, littleEndian);
                                byteOffset += 2;
                                
                                var flags = message.getUint16(byteOffset, littleEndian);
                                byteOffset += 2;
                                var wallMask = message.getUint8(byteOffset);
                                byteOffset += 1;
                                var floor0 = message.getUint8(byteOffset);
                                byteOffset += 1;
                                var floor1 = message.getUint8(byteOffset);
                                byteOffset += 1;
                                var c0 = message.getUint32(byteOffset, littleEndian);
                                byteOffset += 4;
                                var c1 = message.getUint32(byteOffset, littleEndian);
                                byteOffset += 4;
                                
                                var tile = new VesselTile(flags, wallMask, c0, c1, floor0, floor1);
                                
                                chunk.SetTile(new Vec2i(tileLinearI % VesselChunkSize, Math.floor(tileLinearI / VesselChunkSize)), tile);
                            }
                            
                            
                            currentVessel.chunks.Set(chunk.index.x, chunk.index.y, chunk);
                            var diff = SubVec2i(chunk.index, myPlayer.chunkI);
                            if ((Math.abs(diff.x) <= PlayerChunkRange) || (Math.abs(diff.y) <= PlayerChunkRange)) {
                                newChunks.push(chunk);
                            }
                            
                        }
                        
                        //Reinstantiate chunks
                        while (newChunks.length > 0) {
                            var existingChunk = currentVessel.chunks.TryGet(newChunks[0].index);
                            if (existingChunk != null) {
                                existingChunk.Destroy();
                            }
                            console.log("Instantiating chunk (" + newChunks[0].index.x + "," + newChunks[0].index.y + ")", newChunks[0]);
                            currentVessel.InstantiateChunk(newChunks.shift());
                        }
                        
                        break;
                    case ServerMessageType.MakeVesselActive://MakeVesselActive
                        var vIndex = message.getUint32(byteOffset, littleEndian);
                        byteOffset += 4;
                        
                        var vessel = vessels[vIndex];
                        
                        if (vessel == null) {
                            console.log("Creating vessel with index " + vIndex);
                            vessel = new Vessel(vIndex);
                            vessels[vIndex] = vessel;
                        }
                        
                        if (currentVessel != null) {
                            //clear currentVessel information
                            ClearCurrentVessel();
                        }
                        
                        currentVessel = vessel;
                        
                        var otherPlayerCount = message.getUint16(byteOffset, littleEndian);
                        byteOffset += 2;
                        for(var i = 0; i < otherPlayerCount;i++) {
                            //add the ith player
                            var x = message.getFloat32(byteOffset, littleEndian);
                            byteOffset += 4;
                            var y = message.getFloat32(byteOffset, littleEndian);
                            byteOffset += 4;
                            players.push(new Player(new THREE.Vector2(x,y)));
                        }
                        //add ourselves
                        var x = message.getFloat32(byteOffset, littleEndian);
                        byteOffset += 4;
                        var y = message.getFloat32(byteOffset, littleEndian);
                        byteOffset += 4;
                        myPlayer = new Player(new THREE.Vector2(x,y));
                        myPlayer.chunkI = WorldToChunkI(myPlayer.pos);
                        
                        UpdateChunks(true);
                        break;
                    default:
                        console.log("unrecognized message type from server");
                        break;
                }
            }
        }
        //send messages back
    }
    
    if (myPlayer != null && currentVessel != null) {
        UpdateChunks(false);
    }
    
    requestAnimationFrame( render );
    renderer.render( scene, camera );
}
render();
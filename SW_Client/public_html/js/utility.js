
var server_address;
var socket = null;
var socket_started = false;
var messages_out = [];
var messages_in = [];

var Connect = function(address)
{
    server_address = address;
    socket = new WebSocket('ws://' + address);
    socket.binaryType = 'arraybuffer';

    // Open the socket
    socket.onopen = function(event) {
        
        console.log('Connection established with server at: ' + server_address);

        socket_started = true;

        while (messages_out.length > 0){
            socket.send(messages_out.shift());
        }

        // Listen for messages
        socket.onmessage = function(event) {
            var data = event.data;
            messages_in.push(data);
        };

        // Listen for socket closes
        socket.onclose = function(event) {
            console.log('Client notified socket has closed',event);
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
   socket.close();
   socket = null;
   socket_started = false;
   messages_out = [];
   message_in = [];
};
var Receive = function()
{
    if(messages_in.length > 0){
        return messages_in.shift();
    } else {
            return null;
    }
};





var floorTexResolution = 256.0;

//dim0 - floor0 type
//dim1 - floor1 type
//dim2 - wall type
//dim3 - offset along wall
var floorMeshes = [];

var floorUVRects = [];
floorUVRects.push(new Rect(0.0, 0.0, 32.0, 32.0));
floorUVRects.push(new Rect(32.0, 0.0, 32.0, 32.0));

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

var VesselChunkSize = 8;
var VesselChunkSizeF = parseFloat(VesselChunkSize);

var VesselChunkDataLength = VesselChunkSize * VesselChunkSize;




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
    for(i = 0; i < VesselChunkDataLength; i++) {
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
    this.index = index;
    this.version = version;
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
        (localPosition.x < 0.0) ? -1 - parseInt(localPosition.x / -VesselChunkSizeF) : parseInt(localPosition.x / VesselChunkSizeF),
        (localPosition.y < 0.0) ? -1 - parseInt(localPosition.y / -VesselChunkSizeF) : parseInt(localPosition.y / VesselChunkSizeF));
};

var TileToChunkI = function(tileI) {
    return new Vec2i(
        (tileI.x >= 0) ? tileI.x / VesselChunkSize : ((tileI.x + 1) / VesselChunkSize) - 1,
        (tileI.y >= 0) ? tileI.y / VesselChunkSize : ((tileI.y + 1) / VesselChunkSize) - 1);
};

var MatToLinear = function(x, y, columns) {
    return (y * columns) + x;
};





var InitializeMeshData = function() {
    
    var scale = 1.0 / floorTexResolution;
    
    for(i = 0; i < floorUVRects.length; i++) {
        var rect = floorUVRects[i];
        rect.x *= scale;
        rect.y *= scale;
        rect.width *= scale;
        rect.height *= scale;
    }
    
    GenerateFloorMeshes();
};

var GenerateFloorMeshes = function() {
    for (i = 0; i < floorUVRects.length; i++) {
        var temp = [];
        for (j = 0; j < floorUVRects.length; j++) {
            temp.push([]);
        }
        floorMeshes.push(temp);
    }
    
    
    for (i = 0; i < floorUVRects.length; i++) {
        for (j = 0; j < floorUVRects.length; j++) {
            for (k = 0; k < 9; k++) {
                floorMeshes[i][j].push(GenerateMeshes(
                    k,
                    (i === 0) ? floorUVRects[i] : floorUVRects[i-1],
                    i === 0,
                    (j === 0) ? floorUVRects[j] : floorUVRects[j-1],
                    j === 0));
            }
        }
    }
};

var AddTriangle = function(i) {
    var start = i.length;
    i.Add(start + 0);
    i.Add(start + 1);
    i.Add(start + 2);
};

var AddQuad = function(i) {
    var start = i.length;
    i.Add(start + 0);
    i.Add(start + 1);
    i.Add(start + 2);
    i.Add(start + 1);
    i.Add(start + 3);
    i.Add(start + 2);
};

var GenerateBaseVertices = function(a) {
    
    output = [];
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,0.0,0.0),
        new THREE.Vector(a.x, a.y)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,1.0,0.0),
        new THREE.Vector(a.x, a.y + a.height)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,0.0,0.0),
        new THREE.Vector(a.x + a.width, a.y)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,1.0,0.0),
        new THREE.Vector(a.x + a.width, a.y + a.height)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,0.5,0.0),
        new THREE.Vector(a.x, a.y + (a.height * 0.5))));
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.5,1.0,0.0),
        new THREE.Vector(a.x + (a.width * 0.5), a.y + a.height)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.5,0.0,0.0),
        new THREE.Vector(a.x + (a.width * 0.5), a.y)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(0.0,0.0,0.0),
        new THREE.Vector(a.x, a.y)));
    
    output.push(new PosUVPair(
        new THREE.Vector3(1.0,0.5,0.0),
        new THREE.Vector(a.x + a.width, a.y + (a.height * 0.5))));
};

var GenerateMeshes = function(type, rightFloor, none0, leftFloor, none1) {
    
    if (none0 && none1) {
        return null;
    }
    
    output = [];
    
    v0 = GenerateBaseVertices(rightFloor);
    v1 = GenerateBaseVertices(leftFloor);
    
    iTemp = [];
    vTemp = [];
    
    switch (type) {
        case 0:
            //wall type None (simple quad)
            if (!none0) {
                vTemp.push(v0[0]);
                vTemp.push(v0[1]);
                vTemp.push(v0[2]);
                vTemp.push(v0[3]);
                AddQuad(iTemp);
            }
            output.push(new FloorMesh(vTemp, iTemp));
        case 1:
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
        case 2:
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
        case 3:
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
        case 4:
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
        case 5:
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
        case 6:
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
        default:
                return null;
    }
    return output;
};

//VesselChunk method definitions

VesselChunk.prototype.SetTile = function(offset, val) {
    this.data[offset.x + (offset.y * VesselChunkSize)] = val;
};

VesselChunk.prototype.TileAt = function(offset) {
    return this.data[offset.x + (offset.y * VesselChunkSize)];
};

VesselChunk.prototype.OriginTileIndex = function() {
    return new Vec2i(
        this.index.x * VesselChunkSize,
        this.index.y * VesselChunkSize);
};

//Vessel method definitions

VesselTile.prototype.ContainsWall = function(tile, wall) {
    return (tile.wallMask & (1 << (wall - 1))) > 0;
};

VesselTile.prototype.ContainsWallMask = function(tile, wall) {
    return (tile.wallMask & wall) > 0;
};

Vessel.prototype.IsWallLegal = function(index, type)
{
    //check if walls nodes are not too close
    //to the new wall
    if (!(type === 1 || type === 5)) {

        var hDir = (type < 5) ? 1 : -1;
        var diff = Math.abs(type - 5);

        if (hDir < 0) {
            if (this.ContainsWall(new Vec2i(index.x+hDir,index.y))) {
                return false;
            }
            if (diff !== 2) {
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
            if (diff !== 2) {
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
		
    if (tile !== null) {
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
    if (tile !== null) {
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

    for (i = 1; i < 9; i++) {
        tile = TryGetTile(SubVec2i(index, wallOffsets[i]));
        if (tile !== null) {
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
    if (tile !== null) {
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

    for (i = 1; i < 9; i++) {
        tile = TryGetTile(SubVec2i(index, wallOffsets[i]));
        if (tile !== null) {
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
        if (tile.Contains(3 | 2 | 4)) {
            return true;
        }
    }
    //1
    tile = TryGetTile(new Vec2i(index.x, index.y-1));
    if (tile != null) {
        if (tile.Contains(3 | 4 | 7 | 6 | 2 | 8)) {
            return true;
        }
    }
    //2
    tile = TryGetTile(new Vec2i(index.x+1, index.y));
    if (tile != null) {
        if (tile.Contains(7 | 8 | 6)) {
            return true;
        }
    }
    //3
    tile = TryGetTile(new Vec2i(index.x-1, index.y-1));
    if (tile != null) {
        if (tile.Contains(4 | 2)) {
            return true;
        }
    }
    //4
    tile = TryGetTile(new Vec2i(index.x+1, index.y-1));
    if (tile != null) {
        if (tile.Contains(8 | 6)) {
            return true;
        }
    }
    //5
    tile = TryGetTile(new Vec2i(index.x-2, index.y));
    if (tile != null) {
        if (tile.Contains(2)) {
            return true;
        }
    }
    //6
    tile = TryGetTile(new Vec2i(index.x+2, index.y));
    if (tile != null) {
        if (tile.Contains(8)) {
            return true;
        }
    }
    //7
    tile = TryGetTile(new Vec2i(index.x, index.y-2));
    if (tile != null) {
        if (tile.Contains(6 | 4)) {
            return true;
        }
    }

    return false;
};

Vessel.prototype.NonAcuteSequence = function(wall0, wall1) {
    if (wall1 < 5) {
        if (wall0 > (4 + wall1)) {
            return false;
        }
    } else if (wall1 > 5) {
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

Vessel.prototype.InstantiateChunk = function(chunk)
{
    var chunkOffset = ChunkIToLocal(chunk.index);
    chunk.Instantiate(
            Top(chunk), 
            Left(chunk), 
            Right(chunk), 
            Bottom(chunk), 
            BottomRight(chunk), 
            chunkOffset);
};



//DynamicArray2D method definitions

DynamicArray2D.prototype.GrowTopRight = function(amount) {
    
    var newDim = AddVec2i(this.dim, amount);
    var newData = [];
    for(i = 0; i < (newDim.x * newDim.y);i++) {
        newData.push(null);
    }
    
    for(i = 0; i < this.dim.y;i++) {
        for(j = 0; j < this.dim.x;j++) {
            newData[MatToLinear(j, i, newDim.x)] = this.data[MatToLinear(j, i, this.dim.x)];
        }
    }
    
    this.data = newData;
    this.dim = newDim;
};

DynamicArray2D.prototype.GrowBottomLeft = function(amount) {
    
    var newDim = AddVec2i(this.dim, amount);
    var newData = [];
    for(i = 0; i < (newDim.x * newDim.y);i++) {
        newData.push(null);
    }
    
    for(i = amount.y; i < newDim.y;i++) {
        for(j = amount.x; j < newDim.x;j++) {
            newData[MatToLinear(j, i, newDim.x)] = this.data[MatToLinear(j - amount.x, i - amount.y, this.dim.x)];
        }
    }
    
    this.origin = SubVec2i(this.origin, amount);
    this.data = newData;
    this.dim = newDim;
};

DynamicArray2D.prototype.Contains = function(index) {
    
    return (index.x >= this.origin.x && index.x < this.dim.x + this.origin.x) && (index.y >= this.origin.y && index.y < this.dim.y + this.origin.y);
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

DynamicArray2D.prototype.TryGet = function(x, y) {
    
    
};

Vessel.prototype.NonAcuteSequence = function(wall0, wall1) {
    
};

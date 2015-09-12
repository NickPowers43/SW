
var LibraryMySocket = {
	SendMessage: function(dPtr, length) {
		var dV = new DataView(new ArrayBuffer(length));
        var heapBytes= new Uint8Array(Module.HEAPU8.buffer, dPtr, length);
		for (var i = 0; i < length; i++) {
			dV.setUint8(i,heapBytes[i]);
		}
		window.mySocket.send(dV.buffer);
	},
	CloseConnection: function(dPtr, length) {
		window.mySocket.close();
	},
	PrintRefresh: function() {
		console.log("PrintRefresh called");
	},
};

//autoAddDeps(LibraryMySocket);
mergeInto(LibraryManager.library, LibraryMySocket);

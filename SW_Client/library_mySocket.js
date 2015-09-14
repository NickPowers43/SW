
var LibraryMySocket = {
	PrintMessage: function(cStr) {
		console.log(Pointer_stringify(cStr));
	},
	GetFunctions: function() {
		window.HandleMessage = Module.cwrap('HandleMessage', '', ['number', 'number']);
		window.HandleClose = Module.cwrap('HandleClose', '');
		window.OnConnectionMade = Module.cwrap('OnConnectionMade', '');
	},
	SendMessage: function(dPtr, length) {
		var dV = new DataView(new ArrayBuffer(length));
        var heapBytes = new Uint8Array(Module.HEAPU8.buffer, dPtr, length);
		for (var i = 0; i < length; i++) {
			dV.setUint8(i,heapBytes[i]);
		}
		window.mySocket.send(dV.buffer);
		console.log("sending message length: ", length, heapBytes);
	},
	CloseConnection: function() {
		window.mySocket.close();
	},
	PrintRefresh: function() {
		console.log("PrintRefresh called");
	},
};

//autoAddDeps(LibraryMySocket);
mergeInto(LibraryManager.library, LibraryMySocket);


var LibraryMySocket = {
	StartProgram: function() {
		console.log("running StartProgram");
		
		
		window.Initialize = Module.cwrap('Initialize', 'number', ['number', 'number']);
		window.Update = Module.cwrap('Update', '');
		window.HandleMessage = Module.cwrap('HandleMessage', '', ['number', 'number']);
		window.HandleClose = Module.cwrap('HandleClose', '');
		window.OnConnectionMade = Module.cwrap('OnConnectionMade', '');
		
		window.Initialize(window.canvasWidth, window.canvasHeight);
		window.render = function () {
			window.Update();
			window.requestAnimationFrame(window.render);
		};
		window.render();
	},
	PrintMessage: function(cStr) {
		console.log(Pointer_stringify(cStr));
	},
	SendMessage: function(dPtr, length) {
		var dV = new DataView(new ArrayBuffer(length));
        var heapBytes = new Uint8Array(Module.HEAPU8.buffer, dPtr, length);
		for (var i = 0; i < length; i++) {
			dV.setUint8(i,heapBytes[i]);
		}
		window.mySocket.send(dV.buffer);
		//console.log("sending message length: ", length, heapBytes);
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

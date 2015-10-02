
var LibraryMySocket = {
	StartProgram: function() {
		console.log("running StartProgram");
		
		
		window.Initialize = Module.cwrap('Initialize', 'number', ['number', 'number']);
		window.Update = Module.cwrap('Update', '');
		window.Login = Module.cwrap('Login', '', ['number', 'number']);
		window.LoginAsGuest = Module.cwrap('LoginAsGuest', '');
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
	DisplayMainMenu: function() {
		//var usernameLine = document.createElement("div");
		//usernameLine.setAttribute('align',"middle");
		
		//var passwordLine = document.createElement("div");
		//passwordLine.setAttribute('align',"middle");
		
		var submitLine = document.createElement("div");
		submitLine.setAttribute('align',"middle");
		submitLine.setAttribute('style',"position: relative; z-index: 1;");
		
		var loginButton = document.createElement("input");
		loginButton.setAttribute('type',"submit");
		loginButton.setAttribute('value',"Login");
		var guestButton = document.createElement("input");
		guestButton.setAttribute('type',"submit");
		guestButton.setAttribute('value',"Guest");
		
		submitLine.appendChild(loginButton);
		submitLine.appendChild(guestButton);
		
		//var usernameLabel = document.createElement("input");
		//usernameLine.setAttribute('type',"text");
		//usernameLine.setAttribute('name',"username");
		window.canvas.appendChild(submitLine);
	},
	HideMainMenu: function() {
		while (window.canvas.firstChild) {
			window.canvas.removeChild(window.canvas.firstChild);
		}
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

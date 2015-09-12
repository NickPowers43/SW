
//static Networking functions
window.Connect = function(address)
{
    window.mySocket = new WebSocket('ws://' + address);
    window.mySocket.binaryType = 'arraybuffer';

    // Open the Network.socket
    /*Network.socket.onopen = function(event) {
        
        console.log('Connection established with server at: ' + Network.server_address);
        GameData.connected = true;

        while (Network.messages_out.length > 0){
            Network.socket.send(Network.messages_out.shift());
        }

        // Listen for messages
        //Network.socket.onmessage = function(event) {
        //    var buf = Module._malloc(event.data.length);
        //    Module.HEAPU8.set(event.data, buf);
        //    window.HandleReceiveMessage(buf, event.data.length);
        //    Module._free(buf);
        //};

        // Listen for Network.socket closes
        Network.socket.onclose = function(event) {
            window.HandleConnectionClose();
            Close();
        };
    };*/
};

var UpdateCamera = function(){
    //if (WGL.camera == null){
    //    WGL.camera = new THREE.OrthographicCamera(0, 1, 0.5, -0.5, -1.0, 1.0 );
    //    WGL.camera.position.setZ(0.0);
    //}
    //var width2 = (WGL.viewWidth / WGL.viewHeight) * 0.5;
    //WGL.camera.left = WGL.cameraPos.x + (width2 * -WGL.cameraZoom);
    //WGL.camera.right = WGL.cameraPos.x + (width2 * WGL.cameraZoom);
    //WGL.camera.top = WGL.cameraPos.y + (0.5 * WGL.cameraZoom);
    //WGL.camera.bottom = WGL.cameraPos.y + (-0.5 * WGL.cameraZoom);
    
    //WGL.camera.updateProjectionMatrix();
};

var BuildModeKeyDownListener = function(e) {
    if (e.keyCode == 87) {//W
        //WGL.cameraPos = WGL.cameraPos.clone().add(new THREE.Vector2(0.0, 1.0));
    } else if (e.keyCode == 83) {//S
        //WGL.cameraPos = WGL.cameraPos.clone().add(new THREE.Vector2(0.0, -1.0));
    } else if (e.keyCode == 65) {//A
        //WGL.cameraPos = WGL.cameraPos.clone().add(new THREE.Vector2(-1.0, 0.0));
    } else if (e.keyCode == 68) {//D
        //WGL.cameraPos = WGL.cameraPos.clone().add(new THREE.Vector2(1.0, 0.0));
    }
};
var BuildModeMouseDownListener = function(e) {
    
};
var BuildModeMouseMoveListener = function(e) {
    
};

function ActivateBuildMode() {
    document.addEventListener("keydown", BuildModeKeyDownListener, false);
    document.addEventListener("mousemove", BuildModeMouseMoveListener, false);
    document.addEventListener("mousedown", BuildModeMouseDownListener, false);
}

function DeactivateBuildMode() {
    document.removeEventListener("keydown", BuildModeKeyDownListener, false);
    document.removeEventListener("mousemove", BuildModeMouseMoveListener, false);
    document.removeEventListener("mousedown", BuildModeMouseDownListener, false);
}

function handleMouseDown(event) {
    mouseDown = true;
    lastMouseX = event.clientX;
    lastMouseY = event.clientY;
}

function handleMouseUp(event) {
    mouseDown = false;
}

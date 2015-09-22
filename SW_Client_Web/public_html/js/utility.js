
//static Networking functions
window.Connect = function(address)
{
    window.mySocket = new WebSocket('ws://' + address);
    window.mySocket.binaryType = 'arraybuffer';

    // Open the Network.socket
    window.mySocket.onopen = function(event) {
        window.OnConnectionMade();
        console.log('Connection established with server at: ' + address);

        // Listen for messages
        window.mySocket.onmessage = function(event) {
            var msgData = new Uint8Array(event.data);
            console.log("Message received. Length: ", msgData.length, msgData);
            var bufPtr = Module._malloc(msgData.length);
            var buffer = new Uint8Array(Module.HEAPU8.buffer, bufPtr, msgData.length);
            buffer.set(msgData);
            window.HandleMessage(bufPtr, msgData.length);
            Module._free(bufPtr);
        };

        // Listen for Network.socket closes
        window.mySocket.onclose = function(event) {
            window.HandleClose();
        };
    };
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

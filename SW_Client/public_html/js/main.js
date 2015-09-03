

//Utility functions



//Scene initialization


camera.position.z = 5;

window.performance.mark('physics_update');

function render() {
    
    var elapsedTime = window.performance.measure('physics_update');
    window.performance.clearMarks();
    window.performance.mark('physics_update');
    if (connected) {
        
        var message = Receive();
        if (message !== null) {
            var byteOffset = 0;
            while(byteOffset != message.byteLength) {
                var messageIndex = message.getUint8(byteOffset);
                switch (messageIndex) {
                    case 0:
                        
                        break;
                    default:
                        console.log("unrecognized message type from server");
                        break;
                }
            }
        }
        //send messages back
    }
    
    requestAnimationFrame( render );
    renderer.render( scene, camera );
}
render();


var MyPlugin = {
    Connect: function(ip, port)
    {
    	window.socket_started = false;
    	window.messages_out = [];
    	window.messages_in = [];
    
        window.socket = new WebSocket('ws://' + Pointer_stringify(ip) + ':' + Pointer_stringify(port));
        window.socket.binaryType = 'arraybuffer';
        
        // Open the socket
		window.socket.onopen = function(event) {
		
			window.socket_started = true;
			
			while (window.messages_out.length > 0){
				window.socket.send(window.messages_out.shift());
			}
			
			// Listen for messages
			window.socket.onmessage = function(event) {
				var data = event.data;
				var msg = {};
				u8a = new Uint8Array(data);
				msg.length = u8a.length;
				msg.p = _malloc(msg.length);
				for(var i=0;i<msg.length;i++)
					HEAPU8[msg.p + i] = u8a[i];
				
				window.messages_in.push(msg);
			};
			
			// Listen for socket closes
			window.socket.onclose = function(event) {
				console.log('Client notified socket has closed',event);
			};
		}
    },
    Send: function(data, size)
    {
    	ab = new ArrayBuffer(size);
    	u8a = new Uint8Array(ab);
    	for(var i=0;i<size;i++)
    		u8a[i] = HEAPU8[data + i];
    
    	if(window.socket_started)
    	{
        	window.socket.send(ab);
    	}
    	else
    	{
    		window.messages_out.push(ab);
    	}
    },
    Close: function()
    {
       window.socket.close();
    },
    Receive: function(buffer, bufferSize)
    {
    	if(window.messages_in.length > 0){
	    	var msg = window.messages_in.shift();
	    	for(var i=0;i<msg.length;i++)
				if(i < bufferSize)
					HEAPU8[buffer + i] = HEAPU8[msg.p + i];
	    	return msg.length;
    	} else {
    		return 0;
    	}
    }
};

mergeInto(LibraryManager.library, MyPlugin);


mergeInto(LibraryManager.library, {
    SocketSendMessage_: function(data, length) {
        window.SocketSendMessage(data, length);
    },
    SocketCloseConnection_: function() {
        window.SocketCloseConnection();
    },
    JSMain_: function() {
		console.log("calling window.JSMain");
        window.JSMain();
    },
});
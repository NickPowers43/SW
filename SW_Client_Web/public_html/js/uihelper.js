

$("#server_connect").click(function(){
    var server_add = $("#server_address").val();
    window.Connect(server_add);
});

$("#server_disconnect").click(function(){
    Close();
});



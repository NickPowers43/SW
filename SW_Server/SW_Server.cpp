// SW_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;
using namespace SimpleWeb;

int _tmain(int argc, _TCHAR* argv[])
{
	string portS = string((char*)argv[1]);
	int port = stoi(portS);
	string ipS = string((char*)argv[1]);
	int ip = stoi(ipS);

	SocketServer<WS> server(8080, 4);

	auto& echo = server.endpoint["^/echo/?$"];

	//C++14, lambda parameters declared with auto
	//For C++11 use: (shared_ptr<Server<WS>::Connection> connection, shared_ptr<Server<WS>::Message> message)
	echo.onmessage = [&server](shared_ptr<Server<WS>::Connection> connection, shared_ptr<Server<WS>::Message> message) {
		//To receive message from client as string (data_ss.str())
		stringstream data_ss;
		message->data >> data_ss.rdbuf();

		cout << "Server: Message received: \"" << data_ss.str() << "\" from " << (size_t)connection.get() << endl;

		cout << "Server: Sending message \"" << data_ss.str() << "\" to " << (size_t)connection.get() << endl;

		//server.send is an asynchronous function
		server.send(connection, data_ss, [](const boost::system::error_code& ec){
			if (ec) {
				cout << "Server: Error sending message. " <<
					//See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
					"Error: " << ec << ", error message: " << ec.message() << endl;
			}
		});
	};

	return 0;
}


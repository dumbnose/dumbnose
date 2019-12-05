#pragma once

#include <dumbnose/scope_guard.hpp>


namespace dumbnose {

class socket
{
	socket() : port_(0),listen_sock_(INVALID_SOCKET),
				addr_(inet_addr("0.0.0.0"))
	{
	}

	socket(int port, int type, int proto) 
		: port_(port),listen_sock_(INVALID_SOCKET),addr_(inet_addr("0.0.0.0"))
	{
		// initialize windows socket subsystem
		WSADATA wsa_data;
		int result = WSAStartup(MAKEWORD(2,2),&wsa_data);
		if(result!=NO_ERROR) throw result;
	}

	~socket()
	{
		// cleanup
		closesocket(listen_sock_);
		WSACleanup();
	}

	void connect()
	{
		// create the socket
		listen_sock_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listen_sock_==INVALID_SOCKET) throw WSAGetLastError();
		dumbnose::scope_guard closesocket_guard = dumbnose::make_guard(&closesocket,listen_sock_);

		// socket address
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = inet_addr("127.0.0.1");
		service.sin_port = htons(port_);

		// bind the socket
		int result = bind(listen_sock_, (SOCKADDR*) &service, sizeof(service));
		if (result!=NO_ERROR) throw WSAGetLastError();

		// debugging output
		std::cout << "Port assigned: " << port_;

		// success, dismiss the cleanup and closesocket guards
		closesocket_guard.Dismiss();
	}



private:
	int port_;
	unsigned long addr_;
	SOCKET listen_sock_;
};


} // namespace dumbnose
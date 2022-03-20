#include "TcpSocket.h"

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

const TcpSocket::SocketHandle TcpSocket::InvalidSocketHandle = INVALID_SOCKET;

TcpSocket::TcpSocket() : m_socket(InvalidSocketHandle)
{

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == InvalidSocketHandle)
	{
		throw std::string("Error at socket(): ") + std::to_string(WSAGetLastError());
	}
}

TcpSocket::TcpSocket(SocketHandle socketHandle) : m_socket(socketHandle)
{

}

TcpSocket::~TcpSocket()
{
	if (m_socket != InvalidSocketHandle)
	{
		closesocket(m_socket);
	}
}

bool TcpSocket::Listen(uint16_t remotePort)
{
	addrinfo* result = nullptr, hints;
	ZeroMemory(&hints, sizeof(hints));	
	hints.ai_family = AF_INET;			

	int iResult = getaddrinfo(nullptr, std::to_string(remotePort).c_str(), &hints, &result);
	if (iResult != 0)
	{
		freeaddrinfo(result);
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		return false;
	}

	iResult = bind(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		return false;
	}

	
	freeaddrinfo(result);


	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		return false;
	}

	return true;
}

TcpSocket TcpSocket::Accept()
{
	SocketHandle clientSocket = accept(m_socket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		throw std::string("accept failed") + std::to_string(WSAGetLastError());
	}

	return TcpSocket(clientSocket);
}

bool TcpSocket::Connect(const std::string& remoteAddress, uint16_t remotePort)
{
	addrinfo* result = nullptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;

	int iResult = getaddrinfo(NULL, std::to_string(remotePort).c_str(), &hints, &result);
	if (iResult != 0)
	{
		freeaddrinfo(result);
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		return false;
	}

	iResult = connect(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
		return false;
	}

	freeaddrinfo(result);

	return true;
}

bool TcpSocket::Send(const void* data, int size)
{
	for (int sent = 0, result = 0; sent < size; sent += result)
	{
		result = send(m_socket, static_cast<const char*>(data), size, 0);
		if (result < 0)
		{
			std::cerr << "send failed: " << WSAGetLastError() << std::endl;
			return false;
		}
	}

	return true;
}

bool TcpSocket::Receive(void* data, int size, int& recieved)
{
	recieved = recv(m_socket, static_cast<char*>(data), size, 0);
	if (recieved < 0)
	{
		std::cerr << "receive failed: " << WSAGetLastError() << std::endl;
		return false;
	}

	return true;
}

struct WinsocketInitializer
{
	WinsocketInitializer()
	{
		
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		
		if (iResult != 0)
		{
			throw std::string("WSAStartup failed: ") + std::to_string(iResult);
		}
	}

	~WinsocketInitializer()
	{
		WSACleanup();
	}
};

WinsocketInitializer globalInitializer;
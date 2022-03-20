#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include "../Network/TcpSocket.h"

class ServerConnection
{
private:
	TcpSocket m_socket;
	const int m_fileAvailable = 200;
	const int m_fileNotfound = 404;
	bool m_clientClose;
public:
	ServerConnection() = default;

	void SetSocket(const TcpSocket& socket);

	void ConnectServer();
	void SendMessageToServer();
	void SendFile();
	void SendMessageToServer(const std::string& message);
	void ReceiveFile(const std::string& fileName);
};


#pragma once
#include <iostream>
#include <array>
#include <sstream>
#include <vector>
#include "../Network/TcpSocket.h"
#include <fstream>
class ClientConnection
{
private:
	TcpSocket m_client;
	bool m_clientClose;
	const int m_fileAvailable = 200;
	const int m_fileNotfound = 404;
public:
	ClientConnection() = default;
	void SetClient(TcpSocket&& client);
	void ReceiveFile(const std::string& fileName);
	std::string ReceiveMessage();
	void SendFile();

};


#include "ServerConnection.h"

void ServerConnection::SetSocket(const TcpSocket& socket)
{
	m_socket = socket;
}

void ServerConnection::ConnectServer()
{
	std::cout << "Connecting to server at: " << "127.0.0.1" << std::endl;
	m_socket.Connect("127.0.0.1", 27015);
}


void ServerConnection::SendMessageToServer()
{
	std::cout << "Enter your message : ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::string message;
	getline(std::cin, message);
	std::cout << "Sending message: " << message << std::endl;
	m_socket.Send(message.c_str(), message.size());

	std::array<char, 512> receiveBuffer;
	int received;
	m_socket.Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::cout << "Received: ";
	std::copy(receiveBuffer.begin(), receiveBuffer.begin() + received, std::ostream_iterator<char>(std::cout, ""));
	std::cout << std::endl;
}

void ServerConnection::SendFile()
{
	m_clientClose = false;
	char fileRequested[FILENAME_MAX];
	std::array<char, 1024>bufferFile;
	std::ifstream file;

	int revived;

	memset(fileRequested, 0, FILENAME_MAX);
	int byRecv = m_socket.Receive(fileRequested, FILENAME_MAX, revived);

	if (byRecv == 0 || byRecv == -1) 
	{
		m_clientClose = true;
	}


	file.open(fileRequested, std::ios::binary);

	if (file.is_open() && !m_clientClose) 
	{


		int bySendinfo = m_socket.Send((char*)&m_fileAvailable, sizeof(int));
		if (bySendinfo == 0 || bySendinfo == -1) 
		{
			m_clientClose = true;
		}

		file.seekg(0, std::ios::end);
		long fileSize = file.tellg();


		bySendinfo = m_socket.Send((char*)&fileSize, sizeof(long));
		if (bySendinfo == 0 || bySendinfo == -1) 
		{
			m_clientClose = true;
		}
		file.seekg(0, std::ios::beg);

		do 
		{

			file.read(bufferFile.data(), bufferFile.size());
			if (file.gcount() > 0)
				bySendinfo = m_socket.Send(bufferFile.data(), file.gcount());

			if (bySendinfo == 0 || bySendinfo == -1) 
			{
				m_clientClose = true;
				break;
			}
		} while (file.gcount() > 0);
		file.close();

	}
	else
	{

		int bySendCode = m_socket.Send((char*)&m_fileNotfound, sizeof(int));
		if (bySendCode == 0 || bySendCode == -1)
		{
			m_clientClose = true;
		}

	}

}

void ServerConnection::ReceiveFile(const std::string& fileName)
{
	
	std::array<char, 1024>bufferFile;
	std::ofstream file;
	int byRecv;
	m_clientClose = false;
	int codeAvailable = 404;
	long fileRequestedsize = 0;
	int received;
	int fileDownloaded = 0;

	if (!m_clientClose) 
	{
		byRecv = m_socket.Send(fileName.c_str(), fileName.size());
		if (byRecv == 0 || byRecv == -1) 
		{
			m_clientClose = true;
		}

		byRecv = m_socket.Receive((char*)&codeAvailable, sizeof(int), received);
		if (byRecv == 0 || byRecv == -1) 
		{
			m_clientClose = true;
		}

		if (codeAvailable == 200) {
			byRecv = m_socket.Receive((char*)&fileRequestedsize, sizeof(long), received);
			if (byRecv == 0 || byRecv == -1) 
			{
				m_clientClose = true;
			}

			file.open(fileName.c_str(), std::ios::binary | std::ios::trunc);

			do 
			{

				memset(bufferFile.data(), 0, bufferFile.size());
				byRecv = m_socket.Receive(bufferFile.data(), bufferFile.size(), received);


				if (byRecv == 0 || byRecv == -1) 
				{
					m_clientClose = true;
				}

				file.write(bufferFile.data(), received);
				fileDownloaded += received;
			} while (fileDownloaded < fileRequestedsize);
			file.close();
		}

		else if (codeAvailable == 404) 
		{
			std::cout << "Can't open file or file not found!" << std::endl;
		}
		memset(bufferFile.data(), 0, bufferFile.size());
	}

}



void ServerConnection::SendMessageToServer(const std::string& message)
{

	std::cout << "Sending message: " << message << std::endl;
	m_socket.Send(message.c_str(), message.size());

	std::array<char, 512> receiveBuffer;
	int received;
	m_socket.Receive(receiveBuffer.data(), receiveBuffer.size(), received);
	std::cout << "Received: ";
	std::copy(receiveBuffer.begin(), receiveBuffer.begin() + received, std::ostream_iterator<char>(std::cout, ""));
	std::cout << std::endl;
}
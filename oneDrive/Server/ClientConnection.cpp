#include "ClientConnection.h"

void ClientConnection::SetClient(TcpSocket&& client)
{

	m_client = client;

}


void ClientConnection::ReceiveFile(const std::string& fileName)
{
	
	std::array<char, 1024>bufferFile;
	std::ofstream file;
	int byRecv;
	m_clientClose = false;
	int codeAvailable = 404;
	long fileRequestedsize = 0;
	int received;
	int fileDownloaded = 0;


	byRecv = m_client.Send(fileName.c_str(), FILENAME_MAX);
	if (byRecv == 0 || byRecv == -1)
	{
		m_clientClose = true;
	}

	byRecv = m_client.Receive((char*)&codeAvailable, sizeof(int), received);
	if (byRecv == 0 || byRecv == -1)
	{
		m_clientClose = true;
	}

	if (codeAvailable == 200)
	{
		byRecv = m_client.Receive((char*)&fileRequestedsize, sizeof(long), received);
		if (byRecv == 0 || byRecv == -1) 
		{
			m_clientClose = true;
		}

		file.open(fileName.c_str(), std::ios::binary | std::ios::trunc);

		do 
		{
			memset(bufferFile.data(), 0, bufferFile.size());
			byRecv = m_client.Receive(bufferFile.data(), bufferFile.size(), received);

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

}


std::string ClientConnection::ReceiveMessage()
{
	std::array<char, 512> receiveBuffer;
	int received;
	m_client.Receive(receiveBuffer.data(), receiveBuffer.size(), received);

	std::cout << "Received: ";
	std::copy(receiveBuffer.begin(), receiveBuffer.begin() + received, std::ostream_iterator<char>(std::cout, ""));
	std::cout << std::endl;

	std::string recMessage;

	for (int c = 0; c < received; c++)
	{
		recMessage = recMessage + receiveBuffer[c];
	}

	std::stringstream stream;
	stream << "Thank you, I received " << received << " bytes from you";
	std::string message = stream.str();
	m_client.Send(message.c_str(), message.size());
	std::cout << "Sending: " << message.size() << " bytes" << std::endl;

	return recMessage;
}



void ClientConnection::SendFile()
{
	m_clientClose = false;
	char fileRequested[FILENAME_MAX];
	std::array<char, 1024> bufferFile;
	std::ifstream file;

	int received;

	memset(fileRequested, 0, FILENAME_MAX);
	int byRecv = m_client.Receive(fileRequested, FILENAME_MAX, received);

	if (byRecv == 0 || byRecv == -1) 
	{
		m_clientClose = true;
	}


	file.open(fileRequested, std::ios::binary);

	if (file.is_open() && !m_clientClose)
	{

		int bySendinfo = m_client.Send((char*)&m_fileAvailable, sizeof(int));
		if (bySendinfo == 0 || bySendinfo == -1) 
		{
			m_clientClose = true;
		}

		file.seekg(0, std::ios::end);
		long fileSize = file.tellg();


		bySendinfo = m_client.Send((char*)&fileSize, sizeof(long));
		if (bySendinfo == 0 || bySendinfo == -1) 
		{
			m_clientClose = true;
		}
		file.seekg(0, std::ios::beg);

		do 
		{

			file.read(bufferFile.data(), bufferFile.size());
			if (file.gcount() > 0)
				bySendinfo = m_client.Send(bufferFile.data(), file.gcount());

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

		int bySendCode = m_client.Send((char*)&m_fileNotfound, sizeof(int));
		if (bySendCode == 0 || bySendCode == -1) 
		{
			m_clientClose = true;
		}

	}

}
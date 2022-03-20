#include"ClientConnection.h"

enum class Requests
{
	NONE,
	DOWNLOAD,
	UPLOAD,
	RECEIVE_MESSAGE

};

Requests hashit(const std::string& inString)
{
	if (inString == "none") return Requests::NONE;
	if (inString == "download") return Requests::DOWNLOAD;
	if (inString == "upload") return Requests::UPLOAD;
	if (inString == "message") return Requests::RECEIVE_MESSAGE;
}


int main()
{
	std::cout << "Starting server" << std::endl;

	ClientConnection connectionClient;
	TcpSocket listener;
	listener.Listen(27015);

	std::cout << "Waiting for client to connect" << std::endl;
	TcpSocket client = listener.Accept();

	connectionClient.SetClient(std::move(client));

	std::string request;

	while (true)
	{
		request = connectionClient.ReceiveMessage();

		switch (hashit(request))
		{
		case Requests::NONE:
			break;
		case Requests::DOWNLOAD:
		{
			std::string messageRec = connectionClient.ReceiveMessage();
			int numberOfFiles = stoi(messageRec);
			for (int i = 0; i < numberOfFiles; i++)
			{
				connectionClient.SendFile();
			}
			break;
		}
		case Requests::UPLOAD:
		{
			std::vector<std::string> filesName;
			std::string fileName;
			std::string userName = connectionClient.ReceiveMessage();
			std::string messageRec = connectionClient.ReceiveMessage();
			int numberOfFiles = stoi(messageRec);

			for (int i = 0; i < numberOfFiles; i++)
			{
				messageRec = connectionClient.ReceiveMessage();
			    fileName = ".\\" + userName + ".\\" + messageRec;
				filesName.push_back(fileName);
			}

			messageRec = connectionClient.ReceiveMessage();
			if (messageRec != "empty")
			{
				fileName = ".\\" + userName + ".\\" + messageRec;
				std::remove(fileName.c_str());
			}

			for(const auto& fileNameIt : filesName)
				connectionClient.ReceiveFile(fileNameIt);
	
			break;
		}
		case Requests::RECEIVE_MESSAGE:
			break;
		default:
			break;
		}
	}
	return 0;
}
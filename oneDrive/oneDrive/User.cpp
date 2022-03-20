#include "User.h"
#include "Folder.h"
#include <array>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<thread>

bool CheckUserInFile(const std::string& name)
{
	std::ifstream file("users.txt");
	std::string line;
	while (getline(file, line))
	{
		std::stringstream check(line);
		std::string intermediate;
		getline(check, intermediate, ' ');
		if (intermediate == name)
			return 0;
	}
	file.close();
	return 1;
}


void ChooseOptions(User& user)
{
	std::cout << "Welcome to OneDrive! \n";
	int option;

	do
	{

		std::cout << "\n\nChoose what do you want to do: \n";

		if (!user.GetConnected())
		{
			std::cout << "Enter 1 for creating a new account!\n";
			std::cout << "Enter 2 for login!\n";
		}
		else
		{
			std::cout << "Enter 3 for logout!\n";
			std::cout << "Enter 4 for deleting account!\n";
			std::cout << "Enter 5 for editing your account!\n";
			std::cout << "Enter 6 for creating folder!\n";
			std::cout << "Enter 7 to RUN the application!\n";

		}

		std::cout << "Enter 0 for exit!\n";
		std::cin >> option;

		switch (option)
		{
		case 1:

			user.CreateAccount();
			break;
		case 2:
			user.Login();
			break;
		case 3:
			user.Logout();
			break;
		case 4:
			user.DeleteAccount();
			break;
		case 5:
			user.EditAccount();
			break;
		case 6:
			user.CreateFolder();
			break;
		case 7:
		{
			while (true)
			{
				user.UploadFolder();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			break;
		}
		default:
			break;
		}

	} while (option);

}

/**/
void User::SetPaths()
{
	std::vector<std::string>  tokens = {};
	if (m_connected == 1) {
		std::ifstream file("users.txt");
		std::string line;
		while (getline(file, line))
		{
			std::stringstream check(line);
			std::string intermediate;
			getline(check, intermediate, ' ');
			if (intermediate == m_name)
			{

				while (getline(check, intermediate, ' '))
				{
					tokens.push_back(intermediate);
				}

			}

		}
		if (!tokens.empty())
		{
			tokens[0].erase(0, 1);
			tokens[1].erase(0, 1);
			tokens[0].pop_back();
			tokens[1].pop_back();

			const std::filesystem::path localPath = tokens[0];
			const std::filesystem::path serverPath = tokens[1];
			m_folderLocal.SetPath(localPath);
			m_folderServer.SetPath(serverPath);
		}
	}
	else
	{
		std::cout << "You are not connected, please login first ! ";

	}

}

User::User()
{
	m_serverConnection.ConnectServer();
	m_name = "";
	m_connected = 0;
}

User::User(const std::string& name) :m_name{ name }
{
	m_connected = 0;
}

std::string User::GetName() const
{
	return m_name;
}

Folder User::GetFolderLocal() const
{
	return m_folderLocal;
}

bool User::GetConnected() const
{
	return m_connected;
}


void User::SetFolderLocal(const Folder& folder)
{
	m_folderLocal = folder;
}

void User::SetName(const std::string& name)
{
	m_name = name;
}

bool UserValidation(const std::string& name)
{
	std::regex username_validation("[a-zA-Z]+([a-zA-Z0-9]+)");

	if (!std::regex_match(name, username_validation))
	{
		std::cout << "Invalid type for username.\n";
		std::cout << "Please enter another username to create a new account: ";
		return false;
	}
	return true;
}

void User::CreateAccount()
{
	std::string filename("users.txt");
	std::fstream file;
	std::string name;
	std::cout << "Please enter your username to create a new account: ";
	std::cin >> name;

	while (!UserValidation(name))
		std::cin >> name;

	while (CheckUserInFile(name) == 0)
	{
		std::cout << "The username is already taken, please enter another one: ";
		std::cin >> name;
	}
	file.open(filename, std::ios_base::app | std::ios_base::in);
	if (file.is_open())
		file << name << std::endl;
	SetName(name);
	std::cout << "Your account was created!\n";
}

void User::DeleteAccount()
{
	if (m_connected == 1)
	{
		std::string line;
		std::ifstream file("users.txt");
		if (!file.is_open()) {
			std::cout << "Input file failed to open\n";
		}
		std::ofstream out("outfile.txt");

		while (getline(file, line)) {
			std::stringstream check(line);
			std::string intermediate;
			getline(check, intermediate, ' ');
			if (intermediate != m_name)
			{
				out << line << std::endl;

			}
		}

		DeleteFolder();

		std::cout << "Your account was deleted succesfully!";
		m_connected = 0;
		file.close();
		out.close();
		remove("users.txt");
		rename("outfile.txt", "users.txt");


	}
	else
	{
		std::cout << "You need to login first!";
	}
}

void User::Login()
{
	std::string name;
	std::cout << "Enter your username : ";
	std::cin >> name;
	if (m_connected == 1)
		std::cout << "You are already logged in! \n";
	else
	{
		if (CheckUserInFile(name) == 0)
		{
			m_connected = 1;
			m_name = name;
			std::cout << "You  logged in succesfully! \n";
			SetPaths();


			if (m_folderLocal.GetPath() != "")
			{
				std::cout << "Do you want to download your folder from server?(yes/no) ?\n";
				std::string answer;
				std::cin >> answer;
				if (answer == "yes")
				{
					fs::path tempPath = m_folderLocal.GetPath();
					std::string tempNameFolder = m_folderLocal.GetName();
					m_folderLocal.DeleteFolder();
					Folder tempFolder(tempPath, "");
					DownloadFolder();
				}
				else
				{
					std::cout << "\n Do you want to create another folder ?(yes/no) ?\n";
					std::cin >> answer;
					if (answer == "yes")
					{
						fs::path tempPath = m_folderLocal.GetPath();
						std::string tempNameFolder = m_folderLocal.GetName();
						m_folderLocal.DeleteFolder();
						m_folderLocal.SetPath("");

						tempPath = m_folderServer.GetPath();
						tempNameFolder = m_folderServer.GetName();
						m_folderServer.DeleteFolder();
						m_folderServer.SetPath("");

						std::string line;
						std::ifstream file("users.txt");
						if (!file.is_open()) {
							std::cout << "Input file failed to open\n";
						}
						std::ofstream out("outfile.txt");

						while (getline(file, line)) {
							if (line.substr(0, m_name.size()) == m_name)
							{
								out << m_name << "";

							}
							else
								out << line << std::endl;
						}
						file.close();
						out.close();
						remove("users.txt");
						rename("outfile.txt", "users.txt");

						CreateFolder();
					}
				}

				std::vector<File>files;
				m_folderLocal.Search();
				files = m_folderLocal.GetFiles();
				for (const auto& file : files)
				{
					m_FilesMap.emplace(file.GetName(), static_cast<int>(file.GetSize()));
				}

			}
		}
		else
		{
			std::cout << "This username is not registered. Do you want to create one? (yes/no)";
			std::string answer;
			std::cin >> answer;
			if (answer == "yes")
				CreateAccount();
		}
	}

}

void User::Logout()
{
	std::cout << "Do you want to logout? (yes/no) ";
	std::string answer;
	std::cin >> answer;
	if (answer == "yes" && m_connected == 1)
	{
		m_connected = 0;
		std::cout << "Logout succesfully!";
		m_name = "";
		m_FilesMap.clear();

	}
	else if (m_connected == 0 && answer == "yes")
	{
		std::cout << "You were already disconnected!";
	}
}

void User::EditAccount()
{
	if (m_connected == 1)
	{

		std::string newName;
		do {

			std::cout << "Enter your new username: ";
			std::cin >> newName;
			while (!UserValidation(newName))
				std::cin >> newName;
			if (CheckUserInFile(newName) == 0)
				std::cout << "The username is already taken, please enter another one: ";
		} while (CheckUserInFile(newName) == 0);
		if (newName != m_name)
		{

			std::string line;
			std::ifstream file("users.txt");
			if (!file.is_open()) {
				std::cout << "Input file failed to open\n";
			}
			std::ofstream out("outfile.txt");

			SetPaths();

			while (getline(file, line)) {
				std::stringstream check(line);
				std::string intermediate;
				getline(check, intermediate, ' ');
				if (intermediate == m_name)
				{
					out << newName << " " << m_folderLocal.GetPath() << " " << m_folderServer.GetPath() << " " << std::endl;


				}
				else
					out << line << std::endl;
			}
			file.close();
			out.close();
			remove("users.txt");
			rename("outfile.txt", "users.txt");
			std::cout << "Your username has been updated!\n";
			m_name = newName;
		}

	}
	else
	{
		std::cout << "You need to login first!";
	}
}

void User::CreateFolder()
{
	if (m_connected == 1)
	{
		SetPaths();

		if (m_folderLocal.GetPath() == "")

		{

			std::cout << "Please insert your name for the folder" << std::endl;
			std::string nameFolder;
			std::cin >> nameFolder;
			fs::path path = ".";
			fs::path pathServer = "..\\Server";
			Folder folderLocal(path, "\\" + nameFolder);
			Folder folderServer(pathServer, "\\" + nameFolder);

			m_folderLocal = folderLocal;
			m_folderServer = folderServer;

			std::string line;
			std::ifstream file("users.txt");
			if (!file.is_open()) {
				std::cout << "Input file failed to open\n";
			}
			std::ofstream out("outfile.txt");

			while (getline(file, line)) {
				if (line == m_name)
				{
					out << m_name << " " << m_folderLocal.GetPath() << " " << m_folderServer.GetPath() << std::endl;

				}
				else
					out << line << std::endl;
			}
			file.close();
			out.close();
			remove("users.txt");
			rename("outfile.txt", "users.txt");
		}
		else
		{
			std::cout << "You already have a folder created.";
		}

	}
	else
	{
		std::cout << "You are not connected" << std::endl;
	}
}



void User::DeleteFolder()
{
	if (m_connected)
	{
		if (m_folderLocal.GetPath() == "")
		{
			std::cout << "You need to create a folder first!";
		}
		else
		{
			SetPaths();
			if (m_folderLocal.GetPath() == "")
				return;
			m_folderLocal.DeleteFolder();
			m_folderServer.DeleteFolder();
			m_FilesMap.clear();
			std::cout << "Your folder was deleted successfully.";
		}
	}
	else
	{
		std::cout << "You need to be logged in first!\n";
	}
}


std::string User::FindDeletedFile()
{
	SetPaths();
	std::vector<File>files;
	files = m_folderLocal.GetFiles();

	m_folderLocal.Search();
	std::vector<std::string> tempFileNames1;
	std::vector<std::string> tempFileNames2;

	if (m_FilesMap.size() > files.size())
	{
		for (const auto& fileMap : m_FilesMap)
			tempFileNames1.push_back(fileMap.first);
		for (const auto& file : files)
			tempFileNames2.push_back(file.GetName());


		std::vector<std::string> tempFileNames3(1);
		std::vector<std::string>::iterator  ls, it;
		std::sort(tempFileNames1.begin(), tempFileNames1.end());
		std::sort(tempFileNames2.begin(), tempFileNames2.end());
		ls = std::set_difference(tempFileNames1.begin(), tempFileNames1.end(), tempFileNames2.begin(), tempFileNames2.end(), tempFileNames3.begin());

		m_FilesMap.erase(tempFileNames3[0]);
		return tempFileNames3[0];
	}
	return "empty";
}



void User::UploadFolder()
{
	if (m_connected)
	{
		if (m_folderLocal.GetPath() == "")
		{
			std::cout << "You need to create a folder first!";
		}
		else
		{
			SetPaths();

			std::vector<std::string> fileNamesNew;
			std::vector<File>files;
			m_folderLocal.Search();
			files = m_folderLocal.GetFiles();

			std::string fileToBeDeleted = FindDeletedFile();

			std::cout << fileToBeDeleted << "\n";

			for (int i = 0; i < files.size(); ++i)
			{


				if (files[i].GetSize() != m_FilesMap[files[i].GetName()])
				{
					fileNamesNew.push_back(files[i].GetName());
					m_FilesMap.at(files[i].GetName()) = static_cast<int>(files[i].GetSize());
				}
			}


			m_serverConnection.SendMessageToServer("upload");
			m_serverConnection.SendMessageToServer(m_folderLocal.GetName());

			m_serverConnection.SendMessageToServer(std::to_string(fileNamesNew.size()));
			for (const auto& fileName : fileNamesNew)
			{
				m_serverConnection.SendMessageToServer(fileName);
			}

			m_serverConnection.SendMessageToServer(fileToBeDeleted);

			for (int i = 0; i < fileNamesNew.size(); i++)
			{
				m_serverConnection.SendFile();

			}

		}
	}
	else
	{
		std::cout << "You need to be logged in first!\n";
	}


}

void User::DownloadFolder()
{
	if (m_connected)
	{
		if (m_folderLocal.GetPath() == "")
		{
			std::cout << "You need to create a folder first!";
		}
		else
		{
			SetPaths();
			m_serverConnection.SendMessageToServer("download");
			std::string folderName = m_folderServer.GetName();

			std::vector<std::string>fileNames = m_folderServer.Search();
			m_serverConnection.SendMessageToServer(std::to_string(fileNames.size()));
			for (const auto& fileName : fileNames)
			{
				m_serverConnection.ReceiveFile(".\\" + folderName + ".\\" + fileName);
			}
		}

	}
	else
	{
		std::cout << "You need to be logged in first!\n";
	}

}
#pragma once

#include "Folder.h"

#include <iostream>
#include <string>
#include <fstream>
#include "../Network/TcpSocket.h"
#include "ServerConnection.h"
#include<regex>
#include<unordered_map>

class User
{
private:
	std::string m_name;
	bool m_connected;
	Folder m_folderLocal;
	Folder m_folderServer;
	std::unordered_map<std::string, int> m_FilesMap;
	void SetPaths();
	ServerConnection m_serverConnection;
public:
	User();
	User(const std::string& name);

	std::string GetName() const;
	Folder GetFolderLocal() const;
	bool GetConnected() const;

	void SetFolderLocal(const Folder& folder);
	void SetName(const std::string& name);

	void Login();
	void Logout();
	void CreateAccount();
	void DeleteAccount();
	void EditAccount();
	void CreateFolder();
	void DeleteFolder();
	void UploadFolder();
	void DownloadFolder();
	std::string FindDeletedFile();
};

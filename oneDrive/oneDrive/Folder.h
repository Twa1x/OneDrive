#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include "File.h"
namespace fs = std::filesystem;

class Folder
{
private:
	fs::path m_path;
	std::string m_name;
	uint32_t m_timeStamp;
	uint64_t m_size;
	std::vector<File> m_files;
	fs::file_time_type m_date;
public:
	Folder() = default;
	Folder(const fs::path& path, const std::string& name, const uint64_t& size, const std::vector<File>& files);
	Folder(const fs::path& path,const std::string& name);

	void SetPath(const fs::path& path);
	void SetName(const std::string& name);
	void SetSize();
	void SetVector(const std::vector<File>& file);

	uint32_t GetDateTimeStamp();
	fs::path GetPath()const;
	std::string GetName()const;
	uint64_t  GetSize()const;
	std::vector<File> GetFiles()const;

	std::vector<std::string> Search();
	void DeleteFolder();
};
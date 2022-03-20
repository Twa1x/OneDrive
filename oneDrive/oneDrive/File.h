#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <string>
namespace fs = std::filesystem;


class File
{
private:
	std::string m_name;
	uint64_t m_size;
	fs::file_time_type m_date;
	fs::path m_path;
	uint32_t m_timeStamp;

public:
	File() = default;
	File(const std::string& name, const uint64_t& size, const fs::file_time_type& date, const fs::path& path);
	File(const fs::path& path);
	File(const std::string& fileName,const fs::path& path);


	uint32_t GetDateTimeStamp();
	std::string GetName() const;
	uint64_t GetSize() const;
	uint32_t GetDate() const;
	fs::path GetPath() const;

	void SetName(const std::string& name);
	void SetSize();
	void SetDate(const fs::file_time_type& date);
	void SetPath(const fs::path& path);
};


#include "Folder.h"

std::string getNameFromPath(const fs::path& path)
{

	std::string pathStr = path.string();
	std::string name;
	for (int i = pathStr.size() - 1; i >= 0; i--)
	{
		if (pathStr[i] == '\\')
			break;
		else
			name = name + pathStr[i];
	}
	std::reverse(name.begin(), name.end());
	return name;
}


Folder::Folder(const fs::path& path, const std::string& name, const uint64_t& size, const std::vector<File>& files) : m_path{ path }, m_name{ name }, m_size{ size }, m_files{ files }
{
	m_name = name;
	fs::path tempPath = path;
	tempPath += name;
	m_path = tempPath;
	fs::create_directory(m_path);
	m_date = std::filesystem::last_write_time(m_path);

}

Folder::Folder(const fs::path& path, const std::string& name)
{
	if (name != "")
	{
		fs::path tempPath = path;
		m_name = name;
		tempPath += name;
		m_path = tempPath;
		fs::create_directory(m_path);
		m_date = std::filesystem::last_write_time(m_path);
	}
	else
	{
		m_path = path;
		fs::create_directory(path);
		m_date = std::filesystem::last_write_time(m_path);
	}
}


std::vector<std::string> Folder::Search()
{
	std::vector<std::string>filesName;
		m_files.clear();
	for (auto itEntry = fs::recursive_directory_iterator(m_path); itEntry != fs::recursive_directory_iterator(); itEntry++)
	{
		const auto filenameStr = itEntry->path().filename().string();
		const auto filePath = itEntry->path().string();
		filesName.push_back(filenameStr);
		File file(filenameStr, filePath);
		m_files.push_back(file);
	}
	return filesName;
}

void Folder::SetPath(const fs::path& path)
{
	m_path = path;
}

void Folder::SetName(const std::string& name)
{
	m_name = name;
}

void Folder::SetSize()
{
	for (const auto& f : m_files)
	{
		m_size += f.GetSize();
	}

}

void Folder::SetVector(const std::vector<File>& file)
{
	m_files = file;
}

fs::path Folder::GetPath() const
{
	return m_path;
}

std::string Folder::GetName() const
{
	return getNameFromPath(m_path);
}

uint64_t Folder::GetSize() const
{
	return m_size;
}

std::vector<File> Folder::GetFiles() const
{
	return m_files;
}


uint32_t Folder::GetDateTimeStamp()
{
	const auto fileTime = std::filesystem::last_write_time(m_path);
	const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(m_date);
	const auto time = std::chrono::system_clock::to_time_t(systemTime);
	m_timeStamp = time;
	return time;

}

void Folder::DeleteFolder()
{
	fs::remove_all(m_path);
}
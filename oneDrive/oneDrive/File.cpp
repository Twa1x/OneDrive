#include "File.h"

File::File(const std::string& name, const uint64_t& size, const fs::file_time_type& date, const fs::path& path) :m_name{ name }, m_size{ size }, m_date{ date }, m_path{ path }
{
}

File::File(const fs::path& path) : m_path{ path }
{
	
	m_size = fs::file_size(m_path);
	m_date = std::filesystem::last_write_time(m_path);

}

File::File(const std::string& fileName, const fs::path& path)
{
	m_path = path;
	m_size = fs::file_size(m_path);
	m_date = std::filesystem::last_write_time(m_path);
	m_name = fileName;

}


std::string File::GetName() const
{
	return m_name;
}

uint64_t File::GetSize() const
{
	return m_size;
}

uint32_t File::GetDate() const
{
	return m_timeStamp;
}


fs::path File::GetPath() const
{
	return m_path;
}

void File::SetName(const std::string& name)
{
	m_name = name;
}

void File::SetSize()
{
	m_size = fs::file_size(m_path);
}

void File::SetDate(const fs::file_time_type& date)
{
	m_date = std::filesystem::last_write_time(m_path);
}

uint32_t File::GetDateTimeStamp()
{
	const auto fileTime = std::filesystem::last_write_time(m_path);
	const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(m_date);
	const auto time = std::chrono::system_clock::to_time_t(systemTime);
	m_timeStamp = time;
	return time;

}

void File::SetPath(const fs::path& path)
{
	m_path = path;
}

#include "std_filename.h"

std_filename::std_filename(const std::string& file_path)
: m_path(file_path)
{}

std_filename::~std_filename()
{}

bool std_filename::Exists(const std::string& file_path)
{
   return boost::filesystem::exists(file_path) || boost::filesystem::is_directory(file_path);
}

bool std_filename::FileExists(const std::string& file_path)
{
   return boost::filesystem::exists(file_path);
}

std::string std_filename::GetFullName() const
{
    return m_path.stem().string() + m_path.extension().string();
}

std::string std_filename::GetExt() const
{
   return m_path.extension().string();
}

std::string std_filename::GetName() const
{
   return m_path.stem().string();
}

std::string std_filename::GetPath() const
{
   return m_path.parent_path().string();
}

std::string std_filename::GetFullPath() const
{
   return m_path.string();
}

void std_filename::SetFullName(const std::string& fullname)
{
   m_path = m_path.parent_path() / fullname;
}

void std_filename::SetExt(const std::string& ext)
{
   m_path.replace_extension(ext);
}

// Sets the name part (without extension).
void std_filename::SetName(const std::string& name)
{
    m_path = m_path.parent_path() / boost::filesystem::path(name + m_path.extension().string());
}

void std_filename::SetPath(const std::string& path)
{
    m_path = boost::filesystem::path(path) / boost::filesystem::path(m_path.stem().string() + m_path.extension().string());
}

std::string std_filename::DisplayName(bool show_path)
{
   return ((show_path)? GetFullPath() : GetFullName());
}

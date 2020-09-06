#include "std_filename.h"

std_filename::std_filename()
{}

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

std::vector<std::string> std_filename::GetDirs() const
{
   boost::filesystem::path dir = m_path.parent_path();

   std::vector<std::string> dirs;
   for(auto& p : dir) {
      std::string token = p.string();
      if(token != "\\" && token != "/") dirs.push_back(token);
   }
   return std::move(dirs);
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

void std_filename::RemoveLastDir()
{
   boost::filesystem::path dir = m_path.parent_path();
   dir.remove_filename();
   m_path = dir / boost::filesystem::path(m_path.stem().string() + m_path.extension().string());
}

std::string std_filename::get_current_directory()
{
   return boost::filesystem::current_path().string();
}

void std_filename::set_current_directory(const std::string& dir_path)
{
   boost::filesystem::current_path(dir_path);
}

bool std_filename::create_directory(const std::string& dir_path, bool throw_if_exists)
{
   if(boost::filesystem::exists(dir_path)) {
      if(throw_if_exists) throw std::logic_error("Directory already exists: " + dir_path);
      return true;
   }
   return boost::filesystem::create_directory(dir_path);
}

bool std_filename::create_directories(const std::string& dir_path, bool throw_if_exists)
{
   if(boost::filesystem::exists(dir_path)) {
      if(throw_if_exists) throw std::logic_error("Directory already exists: " + dir_path);
      return true;
   }
   return boost::filesystem::create_directories(dir_path);
}

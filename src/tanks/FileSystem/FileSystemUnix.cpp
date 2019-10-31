#include "FileSystemUnix.h"

#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

namespace FileSystem
{
	//----------------------------------------------------------------------------------------------

	FileSystemUnix::FileSystemUnix(const std::string& rootDirectory)
		: IFileSystem(rootDirectory)
	{
	}

	std::vector<std::string> FileSystemUnix::EnumAllFiles(const std::string &mask)
	{
        std::vector<std::string> files;
        if (DIR *dir = opendir(_rootDirectory.c_str()))
        {
            try
            {
                while ( const dirent *e = readdir(dir) )
                {
                    if( (DT_REG == e->d_type || DT_LNK == e->d_type) && !fnmatch(mask.c_str(), e->d_name, 0) )
                    {
                        files.push_back(e->d_name);
                    }
                }
            }
            catch(const std::exception&)
            {
                closedir(dir);
                throw;
            }
            closedir(dir);
        }
        else
        {
            throw std::runtime_error("open directory");
        }
        
        return files;
	}

	char FileSystemUnix::Separator() const
	{
		return '/';
	}

	std::shared_ptr<IFileSystem> FileSystemUnix::GetFileSystem(const std::string &path, bool create, bool nothrow)
	{
        if( std::shared_ptr<IFileSystem> tmp = IFileSystem::GetFileSystem(path, create, true) )
        {
            return tmp;
        }

        assert(!path.empty());

        // skip delimiters at the beginning
        std::string::size_type offset = path.find_first_not_of('/');
        assert(std::string::npos != offset);

        std::string::size_type p = path.find('/', offset);
        std::string dirName = path.substr(offset, std::string::npos != p ? p - offset : p);
        std::string tmpDir = _rootDirectory + '/' + dirName;

        struct stat s;
        if( stat(tmpDir.c_str(), &s) )
        {
            if( create )
            {
                if( mkdir(tmpDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) )
                {
                    if( nothrow )
                        return nullptr;
                    else
                        throw std::runtime_error("could not create directory");
                }
            }
            else
            {
                if( nothrow )
                    return nullptr;
                else
                    throw std::runtime_error(tmpDir + " - directory not found");
            }
        }
        else if( !S_ISDIR(s.st_mode) )
        {
            if( nothrow )
                return nullptr;
            else
                throw std::runtime_error("not a directory");
        }

        // at this point the directory was either found or created
        std::shared_ptr<IFileSystem> child = std::make_shared<FileSystemUnix>(_rootDirectory + '/' + dirName);
        Mount(dirName, child);
        if( std::string::npos != p )
            return child->GetFileSystem(path.substr(p), create, nothrow); // process the rest of the path
        
        return child; // last path node was processed
	}

	std::shared_ptr<IFileSystem> CreateOSFileSystem(const std::string &rootDirectory)
	{
        std::string pathToData;
        pathToData.append("../");
        pathToData.append(rootDirectory);
        
        char resolved_path[PATH_MAX];
        if (!realpath(pathToData.c_str(), resolved_path))
            throw std::runtime_error("realpath can not resolve path to the data folder");

        return std::make_shared<FileSystemUnix>(resolved_path);
	}
}

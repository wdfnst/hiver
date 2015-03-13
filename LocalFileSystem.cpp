/*************************************************************************
 > File Name: LocalFileSystem.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Thu 01 Jan 2015 06:20:31 PM PST
 ************************************************************************/
#include "LocalFileSystem.h"

namespace pdfs {

Metadata LocalFileSystem::readMetadata(const std::string filename)
{
    Metadata metadata = {"", "", "", "", 0, 0, 0};
    std::ifstream fin("metadata/" + filename, 
            std::ios_base::in | std::ios_base::binary);
    if (!fin.good())
    {
        std::cerr << "Open metadata file failed.\n";
        return metadata;
    }
    fin.read(reinterpret_cast<char*>(&metadata), sizeof(struct Metadata));
    fin.close();
    return metadata;
}

ssize_t LocalFileSystem::writeMetadata(
        Metadata &metadata)
{
    std::ofstream fout(METADATA_FOLDER + std::string(metadata.filename),
            std::ios_base::out | std::ios_base::binary);
    if (!fout.good())
    {
        std::cerr << "Open/Create metafile failed.\n";
        return -1;
    }
    fout.write(reinterpret_cast<char*>(&metadata), sizeof(metadata));
    fout.close();
    return 0;
}

ssize_t LocalFileSystem::checkFileExists(std::string filename)
{
    ssize_t exists = 0;
    std::ifstream fin(filename, std::ifstream::in);
    if (!fin.good())
        exists = -1;
    else
        exists = 1;
    fin.close();
    return exists;
}

// Check directory exists, or create the directory
ssize_t LocalFileSystem::createDir(std::string path)
{
    std::ifstream fin(path, std::ifstream::in);
    if (!fin.good())
    {
        if(mkdir(path.c_str(), 0755) == -1)
        {
            std::cerr << "Create directory (" << 
                path << ") failed.\n";
            return -1;
        }
    }
    fin.close();
    return 0;
}

// Remove File
ssize_t LocalFileSystem::removeFile(std::string filename)
{
    return remove(filename.c_str());
}

// Get File name from path
std::string LocalFileSystem::getBasename(
        const std::string &pathname)
{
    std::string basename = "";
    size_t slashPos = pathname.find_last_of("/");
    if ( slashPos != std::string::npos)
        basename = pathname.substr(slashPos + 1, 
                pathname.size() - slashPos);
    else
        basename = pathname;
    return basename;
}

// Get File size
unsigned int LocalFileSystem::getFileSize(const std::string &pathname)
{ 
    std::ifstream fin(pathname);
    if (!fin.is_open())
    {
        std::cerr << "Open file failed in getFileSize.\n";
        return -1;
    }
    fin.seekg(0, std::ios::end);
    std::streampos ps = fin.tellg();
    fin.close();
    return ps;
}

// Get File MD5
std::string LocalFileSystem::getFileMD5(const std::string &pathname)
{
    MD5 md5;
    return md5.calc(pathname.c_str());
}

std::string LocalFileSystem::getFileOwner(const std::string filename)
{
    // IP:local_owner
    return "someone";
}

size_t LocalFileSystem::writeFile(char *message, ssize_t size)
{
    // Instance the messageheader from a char* pointer
    struct messageheader header;
    memcpy(&header, message, sizeof(struct messageheader));

    // Concat the path of temp to filename
    char filepath[128];
    bzero(filepath, sizeof(filepath));
    memcpy(filepath, UPLOAD_FOLDER.c_str(), UPLOAD_FOLDER.size());
    memcpy(filepath + UPLOAD_FOLDER.size(), header.blockid, 
            sizeof(header.blockid));

    // Open file to write
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(fd, header.blockdatasize);  
    int pwritebytes = 0;
    if ((pwritebytes = pwrite(fd, message + sizeof(struct messageheader),
                    header.msgdatasize, header.msgdataoffset)) == -1)
    {
        perror("pwrite failed.\n");
    }
    close(fd);
    return pwritebytes;
} // end of function writeFile

std::vector<std::string> LocalFileSystem::listDirFiles(const std::string dir)
{
    std::vector<std::string> filenames;

    DIR* dp;
    struct dirent* dirp;
    if((dp = opendir(dir.c_str())) == NULL)
    {
        perror("opendir");
        return std::move(filenames);
    }
    while((dirp = readdir(dp)) != NULL)
    {
        char fullname[255];
        memset(fullname, 0, sizeof(fullname));

        /* ignore hidden files */
        if(dirp->d_name[0] == '.')
            continue;
        filenames.push_back(dirp->d_name);
    }
    return std::move(filenames);
} // end of function listDirFiles
} // end of namepsace pdfs

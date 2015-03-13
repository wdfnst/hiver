/*************************************************************************
 > File Name: LocalFileSystem.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Thu 01 Jan 2015 06:20:24 PM PST
 ************************************************************************/
#ifndef LOCAL_FILE_SYSTEM_H
#define LOCAL_FILE_SYSTEM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <sys/stat.h>

#include "pdfs.h"
#include "MD5.h"

namespace pdfs {
class LocalFileSystem {
    public:
        // File metadata read and write
        static Metadata readMetadata(const std::string filename);
        static ssize_t writeMetadata(Metadata &metadata);

        // File directory and affiliation info
        static ssize_t checkFileExists(std::string filename);
        static ssize_t createDir(std::string path);
        static ssize_t removeFile(std::string filename);
        static unsigned int getFileSize(const std::string &pathname);
        static std::string getBasename(const std::string &pathname);
        static std::string getFileMD5(const std::string &pathname);
        static std::string getFileOwner(const std::string filename);
        static std::vector<std::string> listDirFiles(
                const std::string dir = "metadata/");

        // File content read/write
        size_t writeFile(char *message, ssize_t size);
}; // end of class LocalFileSystem
} // end of namepsace pdfs
#endif

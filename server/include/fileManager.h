#pragma once
#include<sys/mman.h>
#include<string>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include"httpDefinition.h"
class fileManager
{
	//判断请求文件是否为CGI程序
	bool is_CGI;
	size_t fileSize;
	std::string mimeType;
public:
	enum FileStatus
	{
		Not_Found = -2,
		Permission_Denied = -1,
		OK = 0
	};
	char* filePtr;
	fileManager();
	fileManager::FileStatus getFile(std::string);
	size_t getFileSize() const;
	std::string getMime() const;
	fileManager(fileManager&) = delete;
	fileManager(fileManager&&) = delete;
	~fileManager();
};


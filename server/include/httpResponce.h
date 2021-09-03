#pragma once
#include"fileManager.h"
#include"httpDefinition.h"
#include"sstream"
#include<memory>
#include"serverConfig.h"
class httpResponce
{
public:
	const std::string& getHeader();
	httpDefinition::HttpVersion version;
	httpDefinition::HttpStatus statusCode;
	size_t contentLength;
	std::string contentType;
	std::string header;
	std::unique_ptr<fileManager> fm;
	const char* fileData;
	fileManager::FileStatus initFile(std::string URI);
	httpResponce();
	int setVersion(httpDefinition::HttpVersion);
	int clear();
};


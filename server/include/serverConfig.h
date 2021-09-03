#pragma once
#include<string>
#include"httpDefinition.h"
//µ¥ÀýÄ£Ê½
class httpServer;
class serverConfig
{
	serverConfig();
	std::string main_path;
	std::string ipaddr;
	std::string idxFile;
	int port;
	httpDefinition::HttpVersion Max_Support_Http_Version;
	friend class httpServer;
public:
	std::string getPath() const;
	httpDefinition::HttpVersion getVersion() const;
	std::string getAddr() const;
	std::string getIdxFile() const;
	int getPort() const;

	static serverConfig& getConfig();
};


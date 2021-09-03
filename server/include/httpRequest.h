#pragma once
#include"httpDefinition.h"
#include"fileManager.h"
#include<unordered_map>
#include<sstream>
#include<iostream>
class httpRequest
{
public:
	enum RequestState
	{
		Not_Support,
		Bad_Request,
		Good_Request,
		Continue_Request
	};
	httpDefinition::HttpVersion version;
	httpDefinition::HttpMethod method;
	std::string file_path;
	std::string query;
	std::string body;
	size_t content_length;
	bool keep_alive;
	std::unordered_map<std::string, std::string> headerMp;
	void clear();
	void print();
	httpRequest();
};


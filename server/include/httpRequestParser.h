#pragma once
#include<cstring>
#include<string>
#include"httpRequest.h"
#include <algorithm>
constexpr auto Line_Overload = -2;
constexpr auto Line_More = -1;
constexpr auto N_Overload = -1;
class httpRequestParser
{
	char* tem_data;
	size_t tem_size;
	size_t tem_idx;
	const char* buffer;
	size_t buffer_size;
	size_t buffer_idx;
	httpRequest* request;
public:
	httpRequestParser();
	~httpRequestParser();
	httpRequestParser(httpRequestParser&) = delete;
	httpRequestParser(httpRequestParser&&) = delete;
	enum ParserState
	{
		Request_Line_Parser,
		Request_Header_Parser,
		Request_Body_Parser,
		Request_OK
	};
	ParserState parser_state;
	/**
    * @brief 提取一行
    *        成功时返回一行长度,结果放进output中
    *        tem_data容量不足放下一行时返回 Line_Overload
    *        数据不足一行时返回 Line_More,
    */
	int getLine(std::string& output);
	/**
    * @brief 提取n个字符
    *        成功时返回0,结果放进output中
    *        需求字符超过缓存大小返回-1
    *        数据不足一行时返回不足的字符数
    */
	int getn(std::string& output, size_t n=0);
	//设置缓存
	int setBuffer(const char* buffer);
	//重置缓存指针
	int setBufferSize(ssize_t len);
	int setRequest(httpRequest& request);
	static std::string decodeURL(const std::string& url);
	static std::string getURI(const std::string& url);
	static std::string& trim(std::string& str);
	httpRequest::RequestState parseLine(std::string& line);
	httpRequest::RequestState parseHeader_fromLine(std::string& line);
};


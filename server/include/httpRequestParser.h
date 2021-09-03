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
    * @brief ��ȡһ��
    *        �ɹ�ʱ����һ�г���,����Ž�output��
    *        tem_data�����������һ��ʱ���� Line_Overload
    *        ���ݲ���һ��ʱ���� Line_More,
    */
	int getLine(std::string& output);
	/**
    * @brief ��ȡn���ַ�
    *        �ɹ�ʱ����0,����Ž�output��
    *        �����ַ����������С����-1
    *        ���ݲ���һ��ʱ���ز�����ַ���
    */
	int getn(std::string& output, size_t n=0);
	//���û���
	int setBuffer(const char* buffer);
	//���û���ָ��
	int setBufferSize(ssize_t len);
	int setRequest(httpRequest& request);
	static std::string decodeURL(const std::string& url);
	static std::string getURI(const std::string& url);
	static std::string& trim(std::string& str);
	httpRequest::RequestState parseLine(std::string& line);
	httpRequest::RequestState parseHeader_fromLine(std::string& line);
};


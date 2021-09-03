#include "httpRequestParser.h"
#include<iostream>
httpRequestParser::httpRequestParser() :
	tem_data(new char[4096]), tem_idx(0), tem_size(4096),
	parser_state(ParserState::Request_Line_Parser)
{
}

httpRequestParser::~httpRequestParser()
{
	delete[] tem_data;
}

int httpRequestParser::getLine(std::string& output)
{
	char tem;
	while (
		buffer_idx < buffer_size && tem_idx < tem_size)
	{
		tem = buffer[buffer_idx];
		tem_data[tem_idx] = tem;
		if (tem == '\r')
		{
			if (++buffer_idx==buffer_size)
			{
				return Line_More;
			}
			if (buffer[buffer_idx++] == '\n')
			{
				if (tem_idx == tem_size)
				{
					return Line_Overload;
				}
				else
				{
					tem_data[tem_idx] = '\0';
					output += tem_data;
					memset(tem_data, 0, tem_size);
					int ret = tem_idx;
					tem_idx = 0;
					return ret;
				}
			}
		}
		++tem_idx;
		++buffer_idx;
	}
	if (tem_idx >= tem_size)
		return Line_Overload;
	else
		return Line_More;
}

int httpRequestParser::getn(std::string& output, size_t n)
{
	if (n >= tem_size)
		return -1;
	size_t buffer_rest = buffer_size - buffer_idx;
	size_t tem_need = n - tem_idx;
	//缓存中数据字节大于需求量
	if (buffer_rest >= tem_need)
	{
		memcpy(tem_data + tem_idx, buffer + buffer_idx, tem_need);
		buffer_idx += tem_need;
		tem_idx += tem_need;
		tem_data[tem_idx] = '\0';
		output += tem_data;
		memset(tem_data, 0, n);
		tem_idx = 0;
		return 0;
	}
	memcpy(tem_data + tem_idx, buffer + buffer_idx, buffer_rest);
	buffer_idx += buffer_rest;
	return n - buffer_rest;
}

int httpRequestParser::setBuffer(const char* buffer)
{
	this->buffer = buffer;
	return 0;
}

int httpRequestParser::setBufferSize(ssize_t len)
{
	buffer_size = len;
	buffer_idx = 0;
	return 0;
}

int httpRequestParser::setRequest(httpRequest& request)
{
	this->request = &request;
	return 0;
}

std::string httpRequestParser::decodeURL(const std::string& url)
{
	if (url.empty())
		return std::string();
	std::string ret;
	int n = url.size();
	for (int url_index = 0; url_index < n; ++url_index)
	{
		if (url[url_index] == '%')
		{
			if (url_index+2 < n )
			{
				ret += std::stoi(url.substr(++url_index, 2), nullptr, 16);
				++url_index;
			}
			else
				break;
		}
		else
		{
			ret += url[url_index];
		}
	}
	return ret;
}

std::string httpRequestParser::getURI(const std::string& url)
{
	int n = url.size();
	if (!n)
		return std::string();
	int url_idx = 0;
	if (url.substr(url_idx, 7) == "http://")
	{
		url_idx = 7;
		url_idx = url.find('/', url_idx);
		if (url_idx == std::string::npos)
		{
			return std::string();
		}
		return url.substr(url_idx);
	}
	return url;
}

std::string& httpRequestParser::trim(std::string& str)
{
	if (str.empty())
		return str;
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

httpRequest::RequestState httpRequestParser::parseLine(std::string& line)
{
	int n = line.size();
	size_t line_idx = 0;
	//idx指向请求方式
	if (line.find_first_of("GET") == line_idx)
	{
		request->method = httpDefinition::HttpMethod::GET;
	}
	else if (line.find_first_of("POST") == line_idx)
	{
		request->method = httpDefinition::HttpMethod::POST;
	}
	else if (line.find_first_of("PUT") == line_idx)
	{
		request->method = httpDefinition::HttpMethod::PUT;
	}
	else
	{
		request->method = httpDefinition::HttpMethod::METHOD_NOT_SUPPORT;
		return httpRequest::RequestState::Not_Support;
	}
	line_idx = line.find_first_of(" ");
	if (line_idx == std::string::npos)
	{
		return httpRequest::Bad_Request;
	}
	line_idx = line.find_first_not_of(" ", line_idx);
	//idx指向URI
	size_t uri_end = line.find_first_of(" ", line_idx);
	if(uri_end== std::string::npos)
	{
		return httpRequest::Bad_Request;
	}
	std::string URI = line.substr(line_idx, uri_end - line_idx);
	if (URI.empty())
	{
		return httpRequest::Bad_Request;
	}
	URI = getURI(URI);
	URI = decodeURL(URI);
	size_t query_idx = URI.find_first_of('?');
	if (query_idx != std::string::npos)
	{
		request->query = URI.substr(query_idx + 1, uri_end - query_idx - 1);
		request->file_path = URI.substr(0, query_idx);
	}
	else
	{
		request->file_path = URI;
	}
	line_idx = uri_end;
	line_idx = line.find_first_not_of(" ", line_idx);
	//idx指向http version
	std::string http_version = line.substr(line_idx, 8);
	if (http_version == "HTTP/1.1")
	{
		request->version = httpDefinition::HttpVersion::HTTP_11;
		request->keep_alive = true;
	}
	else if (http_version == "HTTP/1.0")
	{
		request->version = httpDefinition::HttpVersion::HTTP_10;
	}
	else if (http_version.find_first_of("HTTP/") != std::string::npos)
	{
		request->version = httpDefinition::HttpVersion::VERSION_NOT_SUPPORT;
	}
	else
	{
		return httpRequest::RequestState::Bad_Request;
	}
	parser_state = Request_Header_Parser;
	return httpRequest::RequestState::Continue_Request;
}

httpRequest::RequestState httpRequestParser::parseHeader_fromLine(std::string& line)
{
	int n = line.size();
	if (!n)
	{
		parser_state = Request_Body_Parser;
		return httpRequest::RequestState::Continue_Request;
	}
	if (n > 200)
		return httpRequest::RequestState::Bad_Request;
	char key[200], value[200];
	std::sscanf(line.data(), "%[^:]:%[^:]", key, value);
	std::string key_s(key);
	std::string value_s(value);
	trim(key_s);
	trim(value_s);
	std::transform(key_s.begin(), key_s.end(), key_s.begin(), toupper);
	request->headerMp[key_s] = value_s;
	if (key_s == "CONTENT-LENGTH")
	{
		request->content_length = std::stoi(value_s);
	}
	if (key_s == "Connection")
	{
		if (value_s == "Keep-Alive")
		{
			request->keep_alive = true;
		}
		else if (value_s == "Close")
		{
			request->keep_alive = false;
		}
	}
	return httpRequest::RequestState::Continue_Request;
}

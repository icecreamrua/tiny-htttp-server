#include "connection.h"

int connection::sendData()
{
	if (send_state == SendState::header)
	{
		if (write_size == 0)
			return 0;
		int rest = write_size - write_idx;
		for (;;)
		{
			int n = ::write(con_socket.getSocketFd(), write_bufferer, rest);
			if (n == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					return rest;
				else if (errno == EINTR)
					continue;
				else
				{
					//发送错误，由工作循环注销该连接
					return -1;
				}
			}
			write_idx += n;
			int ret = write_size - write_idx;
			//发送请求文件数据
			if (ret == 0)
			{
				write_idx = 0;
				write_bufferer = responce->fileData;
				write_size = responce->contentLength;
				send_state = SendState::data;
				break;
			}
			else
			{
				return ret;
			}
		}
	}
	if (send_state == SendState::data)
	{
		if (write_size == 0)
		{
			//非持续连接，由工作循环注销该连接
			if (request->keep_alive == false)
			{
				return -1;
			}
			else
			{
				//初始化以待处理下次请求
				requestParser->parser_state = httpRequestParser::ParserState::Request_Line_Parser;
				request->clear();
				responce->clear();
				return 0;
			}
		}
		int rest = write_size - write_idx;
		for (;;)
		{
			int n = ::write(con_socket.getSocketFd(), write_bufferer, rest);
			if (n == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					return rest;
				else if (errno == EINTR)
					continue;
				else
				{
					//发送错误，由工作循环注销该连接
					return -1;
				}
			}
			write_idx += n;
			int ret = write_size - write_idx;
			if (ret == 0)
			{
				//非持续连接，由工作循环注销该连接
				if (request->keep_alive == false)
				{
					return -1;
				}
				else
				{
					//初始化以待处理下次请求
					requestParser->parser_state = httpRequestParser::ParserState::Request_Line_Parser;
					request->clear();
					responce->clear();
					return 0;
				}
			}
			return ret;
		}
	}
	return 0;
}

int connection::getSock()
{
	return con_socket.getSocketFd();
}

connection::connection(int fd):
	responce(std::make_unique<httpResponce>()),
	request(std::make_unique<httpRequest>()),
	requestParser(std::make_unique<httpRequestParser>()),
	read_bufferer(new char[4096]), read_size(4096),send_state(header)
{
	con_socket.init(fd);
	con_socket.setNonblock();
	requestParser->setRequest(*request);
	requestParser->setBuffer(read_bufferer);
}

int connection::setResponce()
{
	httpDefinition::HttpVersion version = request->version;
	if (version == httpDefinition::HttpVersion::VERSION_NOT_SUPPORT)
		version = serverConfig::getConfig().getVersion();
	responce->setVersion(version);
	//不支持GET以外的请求方式
	if (request->method != httpDefinition::HttpMethod::GET)
	{
		responce->statusCode = httpDefinition::HttpStatus::Not_Implemented;
		return 0;
	}
	responce->initFile(request->file_path);
	write_bufferer = responce->getHeader().data();
	write_idx = 0;
	write_size = responce->getHeader().size();
	send_state = SendState::header;
	return 0;
}

int connection::readData()
{
	int n = 0;
	for (;;)
	{
		n = ::read(con_socket.getSocketFd(),read_bufferer, read_size);
		if (n < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				return 0;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		else if (n > 0)
		{
			requestParser->setBufferSize(n);
			if (requestParser->parser_state == httpRequestParser::ParserState::Request_Line_Parser)
			{
				std::string requestLine;
				int ret = 0;
				//读取请求行
				ret = requestParser->getLine(requestLine);
				if (ret == Line_More)
				{
					return 0;
				}
				else if (ret == Line_Overload)
				{
					return -1;
				}
				if (requestParser->parseLine(requestLine) == httpRequest::RequestState::Bad_Request)
				{
					return -1;
				}
			}
			while (requestParser->parser_state == httpRequestParser::ParserState::Request_Header_Parser)
			{
				std::string requestHead;
				int ret = 0;
				//读取一行请求体
				ret = requestParser->getLine(requestHead);
				if (ret == Line_More)
				{
					return 0;
				}
				else if (ret == Line_Overload)
				{
					return -1;
				}
				if (requestParser->parseHeader_fromLine(requestHead) == httpRequest::RequestState::Bad_Request)
				{
					return -1;
				}
			}
			if (requestParser->parser_state == httpRequestParser::ParserState::Request_Body_Parser)
			{
				if (request->content_length > 0)
				{
					int ret = requestParser->getn(request->body, request->content_length);
					if (ret < 0)
					{
						return -1;
					}
					else if (ret > 0)
					{
						return 0;
					}
				}
				requestParser->parser_state = httpRequestParser::ParserState::Request_OK;
			}
#ifdef DEBUG
			request->print();
#endif // DEBUG
			if (requestParser->parser_state == httpRequestParser::ParserState::Request_OK)
			{
				setResponce();
				int ret = sendData();
				//此次连接错误或完成，返回-1，让工作循环注销该事件
				if (ret == -1)
				{
					return -1;
				}
				//此次请求完成，返回0，不做处理以待接收下次请求
				else if (ret == 0)
				{
					return 0;
				}
				//数据未发送完毕，返回fd，让工作循环注册写入事件
				else
				{
					return con_socket.getSocketFd();
				}
			}
		}
		else
		{
			return -1;
		}
	}
	return 0;
}
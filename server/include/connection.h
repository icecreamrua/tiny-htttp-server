#pragma once
#include"Socket.h"
#include"httpResponce.h"
#include"httpRequest.h"
#include"httpRequestParser.h"
#include"serverConfig.h"
#include<memory>
class connection
{
	//read_bufferer 需初始化
	char* read_bufferer;
	size_t read_idx;
	size_t read_size;
	//从response中获取
	const char* write_bufferer;
	size_t write_idx;
	size_t write_size;
	enum SendState
	{
		header,
		data
	};
	SendState send_state;
	std::unique_ptr<httpResponce> responce;
	std::unique_ptr<httpRequest> request;
	std::unique_ptr <httpRequestParser> requestParser;
	int setResponce();
	connectSocket con_socket;
public:
	/**
    * @brief 将fd缓冲区中的数据读取到read_bufferer,并交由parser处理
    *        返回值为0 继续读取，无需操作
    *        返回值为正整数n 将fd n 注册到epoll 写入事件
	*        返回值为-1 由工作循环注销该连接
    */
	int readData();
	/**
	* @brief 将responce中的数据发送至fd
	*        根据send_state决定发送header或文件数据
	*        返回值为0 发送完成
	*        返回值为正整数n 剩余待发送数据
	*        返回值为-1 由工作循环注销该连接
	*/
	int sendData();
	int getSock();
	connection(int);
	connection() = delete;
	connection(connection&) = delete;
	connection(connection&&) = delete;
};


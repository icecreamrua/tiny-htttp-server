#pragma once
#include<netinet/in.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<unordered_map>
#include<iostream>
#include<cstring>
#include"httperror.h"
#include<sstream>
#include<thread>
#include<string>
#include<arpa/inet.h>
#define MAX_LISTEN_NUM 4096 
//�̲߳���ȫ��RALL socket
class Socket
{
	int socket_fd;
	static std::unordered_map<int, int> fdCount;
	//socket_fd���÷���true,���򷵻�false;
	bool checkSocket();
protected:
	//������
	std::string ip_addr;
	//������
	uint16_t ip_port;
	virtual int setSocket(int fd);
public:
	Socket();
	Socket(Socket&);
	Socket(Socket&&) = default;
	int getSocketFd() const;
	int setNonblock();
	int setReuseAddr();
	int setReusePort();
	virtual std::string getaddr() const = 0;
	virtual uint16_t getport() const = 0;
	~Socket();
	bool close();
};
class listenSocket :public Socket
{
public:
	int init(const std::string& ipaddr = std::string(), uint16_t port = 8080);
	std::string getaddr() const;
	uint16_t getport() const;
};
class connectSocket :public Socket
{
public:
	int init(int fd);
	std::string getaddr() const;
	uint16_t getport() const;
};



#pragma once
#include<iostream>
#include<sys/epoll.h>
#include<vector>
#include<memory>
#include<cstring>
#include<thread>
#include"httperror.h"
#include<sstream>
#include<unistd.h>
class epollManager
{
	int epoll_fd;
	std::vector<epoll_event> ret_events;
	epoll_event tem_ev;
	//epoll_fd可用返回true,否则返回false;
	bool checkEpoll_fd();
public:
	epollManager();
	epollManager(epollManager&) = delete;
	epollManager(epollManager&&) = default;
	enum event_type
	{
		LISTEN_EVENT = EPOLLIN,
		RECEIVE_EVENT = EPOLLIN | EPOLLET,
		SEND_EVENT = EPOLLOUT | EPOLLET
	};
	int registerEvent(int fd, event_type events);
	int deleteEvent(int fd);
	std::vector<epoll_event> poll();
	int init();
};


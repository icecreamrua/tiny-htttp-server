#include "workLoop.h"
#include<memory>
int workLoop::init()
{
	listening.init(serverConfig::getConfig().getAddr(), serverConfig::getConfig().getPort());
	if (em.init() == -1)
		return -1;
	em.registerEvent(listening.getSocketFd(), epollManager::event_type::LISTEN_EVENT);
	return 0;
}
int workLoop::run()
{
	if (init() < 0)
		return -1;
	if (loop() < 0)
		return -1;
	return 0;
}
void workLoop::handleAccept()
{
	int clientFd = accept(listening.getSocketFd(), nullptr, nullptr);
	if (clientFd < 0)
	{
		if (errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EPROTO || errno == EINTR)
			return;
		else
		{
			perror("Failed to accept");
			return;
		}
	}
	auto newCon = std::make_unique<connection>(clientFd);
	em.registerEvent(clientFd, epollManager::event_type::RECEIVE_EVENT);
	fd_to_connection[clientFd] = move(newCon);
	return;
}
void workLoop::handleReceive(std::unique_ptr<connection>& con)
{

	int ret = con->readData();
	if (ret < 0)
	{
		fd_to_connection.erase(con->getSock());
	}
	else if (ret > 0)
	{
		em.registerEvent(con->getSock(), epollManager::event_type::SEND_EVENT);
	}
	return;
}
void workLoop::handleSend(std::unique_ptr<connection>& con)
{
	int ret = con->sendData();
	if (ret < 0)
	{
		fd_to_connection.erase(con->getSock());
	}
	return;
}
int workLoop::loop()
{
	for (;;)
	{
		for (auto& oneEvent : em.poll())
		{
			int fd = oneEvent.data.fd;
			int events = oneEvent.events;
			if (events & (EPOLLERR | EPOLLIN))
			{
				if (fd == listening.getSocketFd())
				{
					handleAccept();
				}
				else
				{
					handleReceive(fd_to_connection[fd]);
				}
			}
			else if (events & EPOLLOUT)
			{
				handleSend(fd_to_connection[fd]);
			}
		}
	}
	return 0;
}


#include "epollManager.h"
using std::cerr;
using std::cout;
using std::endl;
bool epollManager::checkEpoll_fd()
{
	if (epoll_fd < 0)
	{
		httperror("epollManager:Trying to use epoll without initializing");
		return false;
	}
	return true;
}
int epollManager::registerEvent(int fd, event_type event)
{
	if (!checkEpoll_fd())
		return -1;
	tem_ev.data.fd = fd;
	tem_ev.events = static_cast<int>(event);
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &tem_ev);
	memset(&tem_ev, 0, sizeof tem_ev);
	if (ret < 0)
	{
#ifdef DEBUG
		cerr << "epollManager: Failed to add socket fd to epoll" << endl;
#endif // DEBUG
		return -1;
	}
	return 0;
}

int epollManager::deleteEvent(int fd)
{
	if (!checkEpoll_fd())
		return -1;
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
	if (ret < 0)
	{
#ifdef DEBUG
		cerr << "epollManager: Failed to delete socket fd" << endl;
#endif // DEBUG
		return -1;
	}
	return 0;
}

std::vector<epoll_event> epollManager::poll()
{
	if (!checkEpoll_fd())
		return {};
	for (;;)
	{
		int event_count = epoll_wait(epoll_fd, &*ret_events.begin(), ret_events.size(), -1);
		if (event_count == ret_events.size())
			ret_events.resize(ret_events.size() * 2);
		if (event_count < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			std::stringstream info;
			info << "epollManager: epoll_wait() error in"
				<< __FILE__ << "> " << "at " << __LINE__ <<
				" thread: " << getpid();
			httperror(info.str());
			return {};
		}
		else
		{
			return ret_events;
		}
	}
	return {};
}

int epollManager::init()
{
	if (epoll_fd>0)
	{
		httperror("epollManager: Try to initialize epoll for multiple times");
		return 0;
	}
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		std::stringstream info;
		info << "epollManager: Fail to create epoll instance in <"
			<< __FILE__ << "> " << "at " << __LINE__ << 
			" thread: " << getpid();
		httperror(info.str());
		return -1;
	}
	return 0;
}

epollManager::epollManager() :epoll_fd(-1)
{
	ret_events.resize(10);
	memset(&tem_ev, 0, sizeof tem_ev);
}


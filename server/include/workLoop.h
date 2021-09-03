#pragma once
#include"epollManager.h"
#include<unordered_map>
#include"connection.h"
#include"Socket.h"
#include"serverConfig.h"
class workLoop
{
	listenSocket listening;
	std::unordered_map<int, std::unique_ptr<connection>> fd_to_connection;
	epollManager em;
	void handleAccept();
	void handleReceive(std::unique_ptr<connection>&);
	void handleSend(std::unique_ptr<connection>&);
	int loop();
	int init();
public:
	workLoop() = default;
	workLoop(workLoop&&) = default;
	int run();
};


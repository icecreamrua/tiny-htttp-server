#pragma once
#include<string>
#include<sys/stat.h>
#include<iostream>
#include"serverConfig.h"
#include"httpDefinition.h"
#include<errno.h>
#include"workLoop.h"
#include<vector>
#include<memory>
#include<signal.h>
#include<sys/wait.h>
#include <sys/prctl.h>
//����ģʽ
class httpServer
{
	httpServer();
public:
	static httpServer& getServer();
	/**
	* @brief ���÷�������Ŀ¼
	* �ɹ�����0
	* ʧ�ܷ���-1
	* */
	int setPath(const std::string path);
	int setIP(const std::string ipaddr, int port);
	int setIdxFile(const std::string path);
	int init(int max_num=3);
	int run();
};


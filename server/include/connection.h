#pragma once
#include"Socket.h"
#include"httpResponce.h"
#include"httpRequest.h"
#include"httpRequestParser.h"
#include"serverConfig.h"
#include<memory>
class connection
{
	//read_bufferer ���ʼ��
	char* read_bufferer;
	size_t read_idx;
	size_t read_size;
	//��response�л�ȡ
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
    * @brief ��fd�������е����ݶ�ȡ��read_bufferer,������parser����
    *        ����ֵΪ0 ������ȡ���������
    *        ����ֵΪ������n ��fd n ע�ᵽepoll д���¼�
	*        ����ֵΪ-1 �ɹ���ѭ��ע��������
    */
	int readData();
	/**
	* @brief ��responce�е����ݷ�����fd
	*        ����send_state��������header���ļ�����
	*        ����ֵΪ0 �������
	*        ����ֵΪ������n ʣ�����������
	*        ����ֵΪ-1 �ɹ���ѭ��ע��������
	*/
	int sendData();
	int getSock();
	connection(int);
	connection() = delete;
	connection(connection&) = delete;
	connection(connection&&) = delete;
};


#include "fileManager.h"

fileManager::fileManager():is_CGI(false),filePtr(nullptr),fileSize(0), mimeType("text/plain")
{
}

fileManager::FileStatus fileManager::getFile(std::string filePath)
{
	mimeType = httpDefinition::mimeMp[""];
	if (filePath.empty())
	{
		return fileManager::Not_Found;
	}
	int ret = 0;
	//�ж��ļ��Ƿ����
	ret = access(filePath.data(), F_OK);
	if (ret < 0)
	{
		return fileManager::Not_Found;
	}
	//�ж��ļ��Ƿ�ɶ�
	ret = access(filePath.data(), R_OK);
	if (ret < 0)
	{
		return fileManager::Permission_Denied;
	}
	//�ж��ļ�����
	ret = filePath.find_last_of('.');
	if(ret!=std::string::npos)
	{
		auto mime_it = httpDefinition::mimeMp.find(filePath.substr(ret));
		if (mime_it != httpDefinition::mimeMp.end())
		{
			mimeType = mime_it->second;
		}
	}
	//�����ļ���С
	struct stat st;
	stat(filePath.data(), &st);
	fileSize = st.st_size;
	//�����ļ�
	int fileFd = open(filePath.data(), O_RDONLY);
	if (fileFd < 0)
	{
		fileSize = 0;
		return fileManager::Not_Found;
	}
	//ӳ�䵽�ڴ�
	filePtr = (char*)mmap(0, fileSize, PROT_READ, MAP_PRIVATE, fileFd, 0);
	if (filePtr == MAP_FAILED)
	{
		fileSize = 0;
		close(fileFd);
		return fileManager::Not_Found;
	}
	close(fileFd);
	return fileManager::OK;
}

size_t fileManager::getFileSize() const
{
	return fileSize;
}

std::string fileManager::getMime() const
{
	return mimeType;
}

fileManager::~fileManager()
{
	munmap((void*)filePtr, fileSize);
	filePtr = nullptr;
	fileSize = 0;
}

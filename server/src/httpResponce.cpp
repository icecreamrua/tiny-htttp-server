#include "httpResponce.h"

const std::string& httpResponce::getHeader()
{
    if (!header.empty())
        return header;
    std::stringstream ss;
    ss << httpDefinition::httpVersionMp[version] << " "
        << (int)statusCode << " " << httpDefinition::httpStatusMp[statusCode] << "\r\n";
    ss << "Content-type: " << contentType << "\r\n";
    ss << "Content-length: " << contentLength << "\r\n\r\n";
    header= ss.str();
    return header;
}

fileManager::FileStatus httpResponce::initFile(std::string URI)
{
    fileManager::FileStatus ret;
    while (URI.front() == '/')
    {
        URI.erase(URI.begin());
    }
    //·µ»ØÖ÷Ò³
    if (URI.empty())
    {
        URI = serverConfig::getConfig().getIdxFile();
    }
    while (URI.front() == '/')
    {
        URI.erase(URI.begin());
    }
    URI = serverConfig::getConfig().getPath()+URI;
    ret = fm->getFile(URI);
    contentType = fm->getMime();
    if (ret != fileManager::FileStatus::OK)
    {
        if (ret == fileManager::FileStatus::Permission_Denied)
        {
            statusCode = httpDefinition::HttpStatus::Forbidden;
        }
        else if (ret == fileManager::FileStatus::Not_Found)
        {
            statusCode = httpDefinition::HttpStatus::Not_Found;
        }
        return ret;
    }
    fileData = fm->filePtr;
    contentLength = fm->getFileSize();
    statusCode = httpDefinition::HttpStatus::OK;
    return ret;
}

httpResponce::httpResponce():
    version(httpDefinition::HttpVersion::HTTP_10),statusCode(httpDefinition::HttpStatus::OK),
    contentLength(0),contentType("text/plain"), fm(std::make_unique<fileManager>()), fileData(nullptr),header()
{

}

int httpResponce::setVersion(httpDefinition::HttpVersion v)
{
    version = v;
    return 0;
}

int httpResponce::clear()
{
    fm = std::make_unique<fileManager>();
    header.clear();
    fileData = nullptr;
    version = httpDefinition::HttpVersion::HTTP_10;
    statusCode = httpDefinition::HttpStatus::OK;
    contentLength = 0;
    contentType = "text/plain";
    return 0;
}

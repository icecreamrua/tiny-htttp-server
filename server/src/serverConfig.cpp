#include "serverConfig.h"
serverConfig::serverConfig():
    Max_Support_Http_Version(httpDefinition::HttpVersion::HTTP_11),port(0),ipaddr()
{

}
std::string serverConfig::getPath() const
{
    return main_path;
}

httpDefinition::HttpVersion serverConfig::getVersion() const
{
    return Max_Support_Http_Version;
}

std::string serverConfig::getAddr() const
{
    return ipaddr;
}

std::string serverConfig::getIdxFile() const
{
    return idxFile;
}

int serverConfig::getPort() const
{
    return port;
}

serverConfig& serverConfig::getConfig()
{
    static serverConfig config;
    return config;
}

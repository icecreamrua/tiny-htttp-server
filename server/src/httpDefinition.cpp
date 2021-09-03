#include"httpDefinition.h"
std::unordered_map<httpDefinition::HttpStatus, std::string, enumhash>
httpDefinition::httpStatusMp =
{
    {httpDefinition::OK,"OK"},
    {httpDefinition::Bad_Request,"Bad Request"},
    {httpDefinition::Forbidden,"Forbidden"},
    {httpDefinition::Not_Found,"Not Found"},
    {httpDefinition::Internal_Server_Error,"Internal Server Error"},
    {httpDefinition::Not_Implemented,"Not implemented"}
};
std::unordered_map<httpDefinition::HttpVersion, std::string, enumhash>
httpDefinition::httpVersionMp
{
	{httpDefinition::HTTP_10,"HTTP/1.0"},
	{httpDefinition::HTTP_11,"HTTP/1.1"}
};
std::unordered_map<std::string, std::string> httpDefinition::mimeMp
{
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {"", "text/plain"}
};
httpDefinition& httpDefinition::getInstance()
{
	static httpDefinition instance;
	return instance;
}
std::string httpDefinition::getStatus(httpDefinition::HttpStatus n)
{
	return httpStatusMp[n];
}
std::string httpDefinition::getVersion(httpDefinition::HttpVersion n)
{
	return httpVersionMp[n];
}


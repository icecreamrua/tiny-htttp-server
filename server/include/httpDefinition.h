#pragma once
#include<unordered_map>
struct enumhash
{
	template <typename T>
	unsigned int operator() (T t) const
	{
		return static_cast<unsigned int>(t);
	}
};
class httpDefinition
{
public:
	enum HttpStatus
	{
		OK = 200,
		Bad_Request = 400,
		Forbidden = 403,
		Not_Found = 404,
		Internal_Server_Error = 500,
		Not_Implemented =501
	};
	enum HttpVersion
	{
		HTTP_10 =10,
		HTTP_11 =11,
		VERSION_NOT_SUPPORT =0
	};
	enum HttpMethod
	{
		GET,
		POST,
		PUT,
		METHOD_NOT_SUPPORT
	};
	std::string mime_type;
	static httpDefinition& getInstance();
	std::string getStatus(httpDefinition::HttpStatus);
	std::string getVersion(httpDefinition::HttpVersion);
	static std::unordered_map<std::string, std::string> mimeMp;
	static std::unordered_map<httpDefinition::HttpStatus, std::string, enumhash> httpStatusMp;
	static std::unordered_map<httpDefinition::HttpVersion, std::string, enumhash> httpVersionMp;
private:
	httpDefinition()=default;
};


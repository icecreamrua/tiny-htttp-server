#include "httpRequest.h"

void httpRequest::print()
{
	using std::cout;
	using std::endl;
	cout << "method: " << method << endl;
	cout << "URI: " << this->file_path << endl;
	cout << "query " << query << endl;
	cout << "httpversion " << version << endl;
	cout << "content-length " << content_length << endl;
}

void httpRequest::clear()
{
	version = httpDefinition::HttpVersion::VERSION_NOT_SUPPORT;
	method = httpDefinition::HttpMethod::METHOD_NOT_SUPPORT;
	file_path.clear();
	headerMp.clear();
	body.clear();
	query.clear();
	content_length = 0;
}

httpRequest::httpRequest():
	version(httpDefinition::HttpVersion::VERSION_NOT_SUPPORT),
	method(httpDefinition::HttpMethod::METHOD_NOT_SUPPORT),
	content_length(0),
	keep_alive(false)
{
}

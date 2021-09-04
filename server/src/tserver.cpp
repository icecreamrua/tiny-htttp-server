#include<iostream>
#include"httpServer.h"
#include<limits.h>
using namespace std;
string get_currentPath()
{
    string path;
    char current_absolute_path[PATH_MAX];
    int cnt = readlink("/proc/self/exe", current_absolute_path, PATH_MAX);
    if (cnt < 0 || cnt >= PATH_MAX)
    {
        perror("Failed to get path");
        exit(-1);
    }
    int i;
    for (i = cnt; i >= 0; --i)
    {
        if (current_absolute_path[i] == '/')
        {
            current_absolute_path[i + 1] = '\0';
            break;
        }
    }
    path = current_absolute_path;
    return path;
}
int main()
{
	httpServer::getServer().init(0);
	httpServer::getServer().setPath(get_currentPath()+"demo");
	//默认设置
	//httpServer::getServer().setIdxFile("index.html");
	//httpServer::getServer().setIP("0.0.0.0",8081);
	httpServer::getServer().run();
	return 0;
}
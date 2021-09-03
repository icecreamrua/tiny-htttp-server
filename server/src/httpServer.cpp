#include "httpServer.h"
//sigaction实现signal，更好的兼容性
__sighandler_t _Signal(int signo, __sighandler_t func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM)
    {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif // SA_INTERRUPT
    }
    else
    {
        act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return (oact.sa_handler);
}
void handler_sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG))>0)
    {
        std::cerr << "Child " << pid << " terminated" << std::endl;
    }
    return;
}
httpServer::httpServer()
{
    
}
httpServer& httpServer::getServer()
{
    static httpServer server;
    return server;
}

int httpServer::setPath(const std::string path)
{
    std::string main_path = path;
    auto handleError =[]() {
        perror("HttpServer: invalid path");
    };
    if (main_path.back() != '/')
    {
        main_path.push_back('/');
    }
    if (main_path.empty())
    {
        handleError();
        return -1;
    }
    struct stat st;
    if (stat(main_path.data(), &st) < 0)
    {
        handleError();
        return -1;
    }
    if (st.st_mode & S_IRWXU & S_IRUSR)
    {
        serverConfig::getConfig().main_path = main_path;
        return 0;
    }
    handleError();
    return -1;
}

int httpServer::init(int max_num)
{
    setIP("0.0.0.0", 8081);
    setIdxFile("index.html");
    //处理僵死子进程
    _Signal(SIGCHLD, handler_sig_chld);
    //忽略SIGPIPE信号，以避免进程意外结束
    _Signal(SIGPIPE, SIG_IGN);
    for (int i = 0; i < max_num; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            httperror("HttpServer: Failed to init");
            exit(-1);
        }
        //子进程
        if (pid == 0)
        {
            //父进程退出时，收到kill信号以关闭
            prctl(PR_SET_PDEATHSIG, SIGKILL);
            break;
        }
    }
    return 0;
}

int httpServer::run()
{
    if (serverConfig::getConfig().getPath().empty())
    {
        httperror("HttpServer: Failed to run without main path");
        exit(-1);
    }
    workLoop work;
    std::cout << "http server is running....." << std::endl;
    if (work.run() < 0)
        exit(-1);
    return 0;
}

int httpServer::setIP(const std::string ipaddr, int port)
{
    serverConfig::getConfig().ipaddr = ipaddr;
    serverConfig::getConfig().port = port;
    return 0;
}

int httpServer::setIdxFile(const std::string path)
{
    serverConfig::getConfig().idxFile = path;
    return 0;
}

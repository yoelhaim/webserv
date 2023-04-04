#include "cgi.hpp"

CGI::CGI(Request request, Server server, string url, string method) : _request(request) , _server(server) , _url(url) , _method(method)
{
    int pipefd[2];
    int save0 = dup(0);
    int save1 = dup(1);
    getScriptName();
    if (!_args)
        _resp = "";
    else
    {
        pipe(pipefd);
        pid_t pid = fork();
        if (pid == 0) 
        {
            chdir(url.erase(url.find_last_of('/'),url.length()).c_str());
            string body = "" + _request.getBody();
            FILE *tmp = tmpfile();
    	    fputs(body.c_str(), tmp);
        	rewind(tmp);
            dup2(fileno(tmp), 0);
		    close(fileno(tmp));
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            execve(_args[0], _args, NULL);
            exit(1);
        }
        waitpid(pid, NULL, 0);
        close(pipefd[1]);
        char buffer[4096] = "";
        size_t nread;
        while ((nread = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            _resp += buffer;
        
        close(pipefd[0]);
        dup2(save0, STDIN_FILENO);
        dup2(save1,STDOUT_FILENO);
        close(save0);
        close(save1);
    }
}

char **CGI::_envToChar()
{
    char **ret = new char*[_env.size() + 1];
    int i=0;

    for (;i < _env.size(); i++)
    {
        ret[i] = new char[_env[i].size() + 1];
        ret[i] = (char *)_env[i].c_str();
    }
    ret[i] = NULL;
    return ret;
}

string CGI::getResp()
{
    return _resp;
}

char **CGI::setENV()
{
    _env.push_back("REQUEST_METHOD=" + _method);
    _env.push_back("SERVER_SOFTWARE=webserv/1.0");
	_env.push_back("SERVER_NAME=" + _server.getServerName());
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_env.push_back("SERVER_PORT=" + to_string(_server.getPort()));
	_env.push_back("REQUEST_METHOD=" + _request.getMethod());
	_env.push_back("SCRIPT_NAME=" + _scriptName);
	_env.push_back("CONTENT_TYPE=" + _request.getValueOf("Content-Type"));
	_env.push_back("QUERY_STRING=" + _request.getQueryString());
    return (_envToChar());
}

void    CGI::getScriptName()
{
    map<string, string> map = _server._locations[_server.getMatchedLocation()]._directives;
    vector<string> cgi_info;
    char **args = new char*[3];
    _args = NULL;
    if (map.find("cgi_info") != map.end())
        cgi_info = Request::getVector(map["cgi_info"]);
    else
        cgi_info = Request::getVector(_server.getCgiInfo());
    if (cgi_info.empty())
        return ;
    string url = _url;
    string end = url.erase(0, url.find_last_of('.'));
    if (end == ".py")
    {
        if (find(cgi_info.begin(), cgi_info.end(), ".py") != cgi_info.end())
        {
            args[0] = (char *)cgi_info[1].c_str();
            args[1] = (char *)Request::getVector(_url, '/').back().c_str();
            args[2] = NULL;
            _args = args;
        }
    }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matef <matef@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 21:39:23 by matef             #+#    #+#             */
/*   Updated: 2023/04/03 22:39:52 by matef            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

SocketClass::SocketClass()
{
    servers = Configuration().getServers();
}

SocketClass::~SocketClass()
{
}

int SocketClass::create()
{
    int listener;

    listener = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol of socket type
    if (listener < 0)
    {
        cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    return listener;
}

void SocketClass::bindSocket(int listener, SocketServer &server)
{
    bzero(&server.address, sizeof(server.address));
    
    server.address.sin_family = AF_INET;
    server.address.sin_addr.s_addr = INADDR_ANY;
    server.address.sin_port = htons(server.port);

    int opt = 1;

    opt = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (opt < 0)
    {
        cerr << "Error setting socket options" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    int bind_result = ::bind(listener, (struct sockaddr*) &server.address, sizeof(server.address));
    if (bind_result < 0)
    {
        cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

}

void SocketClass::listenSocket(int listener)
{
    if (listen(listener, SOMAXCONN) < 0)
    {
        cerr << "Failed to listen for incoming connections" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SocketClass::acceptSocket(int sockfd)
{
}

int SocketClass::sendFileInPackets(std::string file, struct pollfd &fds)
{
    // cout << "sendFileInPacket" << endl;
    if (_clients[fds.fd].getStatus() == FILE_NOT_SET)
    {
        // _clients[fds.fd].setFile(file);
        _clients[fds.fd].setFileContent(getFileContent(file));
        _clients[fds.fd].setStatus(READY_TO_SEND);
    }

    if (_clients[fds.fd].getStatus() == READY_TO_SEND)
    {
        string mimeType;
        string response;

        mimeType = _mimeTypes.getMimeType(_mimeTypes.getExtension(file));
        response = "HTTP/1.1 200 Ok" ENDL;
        response += "Content-Type: " + mimeType + ENDL;
        response += "Content-Length: " + to_string(getFileSize(file)) + ( ENDL ENDL );
        
        _clients[fds.fd].setStatus(SENDING);
        send(fds.fd, response.c_str(), response.size(), 0);

        return 0;
    }

    string packet = _clients[fds.fd].getPacket();
    send(fds.fd, packet.c_str(), packet.size(), 0);

    return 0;
}

string SocketClass::joinRootAndPath(string root, string path, Request &httpRequest)
{
    if (httpRequest.getRessource() == "/")
        return "./www/html/index.html";

    return "./www/html" + httpRequest.getRessource();
}

bool SocketClass::isHeaderReceived(string request)
{
    return (request.find(ENDL ENDL) != string::npos);
}

bool SocketClass::recvError(int size, int fd)
{
    if ( size < 0 )
    {
        cerr << "recv failed" << endl;
        close(fd);
        return false;
    }
    else if ( !size )
    {
        cerr << "Connection closed" << endl;
        close(fd);
        return false;
    }
    return true;
}

unsigned long SocketClass::hex2dec(string hex)
{
    unsigned long result = 0;

    for (int i=0; i<hex.length(); i++) {
        if (hex[i]>=48 && hex[i]<=57)
        {
            result += (hex[i]-48)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=65 && hex[i]<=70) {
            result += (hex[i]-55)*pow(16,hex.length( )-i-1);
        } else if (hex[i]>=97 && hex[i]<=102) {
            result += (hex[i]-87)*pow(16,hex.length()-i-1);
        }
    }
    return result;
}

string SocketClass::parseChunked(string body)
{
    size_t pos = 0;
    string chunked;
    string size;
    bool isSize = true;
    bool firstLine = true;
    
    string prasedBody;

    while (body.size())
    {
        if (firstLine)
        {
            pos = body.find(ENDL);
            size = body.substr(0, pos);
            body.erase(0, pos + 2);
            firstLine = false;
            if (size == "0") break;
            continue;
        }
        
        prasedBody += body.substr(0, hex2dec(size));
        body.erase(0, hex2dec(size) + 2);
        firstLine = true;
    }

    return prasedBody;
}

void SocketClass::handlePostRequest(Client &client)
{
    if (httpRequest.isHeaderHasKey("Content-Length"))
    {
        if ((int)atof(httpRequest.getValueOf("Content-Length").c_str()) == client.getReceivedLength())
        {
            cerr << "Content-Length: " << httpRequest.getValueOf("Content-Length") << endl;
            client.setBody();
            uploadFile(client);
        }
    }
    else if (httpRequest.isHeaderHasKey("Transfer-Encoding"))
    {
        if (httpRequest.getValueOf("Transfer-Encoding") == "chunked")
        {
            if (client._requestString.rfind(ENDL "0" ENDL ENDL) != string::npos)
            {
                client.setBody();
                parseChunked(client.getBody());
            }
        }
    }
}

void SocketClass::uploadFile(Client &client)
{
    size_t pos_1 = client._requestString.find("\r\n\r\n") + 4;

    client._requestString.erase(0, pos_1);

    vector<string> contentType = Request::getVector(client.getRequest().getValueOf("Content-Type"));
    string boundary = "--" + contentType[1].substr(9);
    string endBoundary = boundary + "--";

    size_t pos = client._requestString.find(boundary);
    size_t pos_2 = client._requestString.find(endBoundary);
    if (pos == string::npos || pos_2 == string::npos) // return msg error : bad request
        return;
    string tmp;

    while (pos != string::npos && pos < pos_2)
    {

        pos_1 = client._requestString.find("\r\n\r\n") + 4;

        string bodyHead = client._requestString.substr(0, pos_1);

        while (bodyHead.find("\r\n") != string::npos)
            bodyHead.replace(bodyHead.find("\r\n"), 2, " ");

        vector <string> bodyHeadVector = Request::getVector(bodyHead);

        while (bodyHeadVector.size() && bodyHeadVector[0].find("filename") == string::npos)
            bodyHeadVector.erase(bodyHeadVector.begin());

        if (!bodyHeadVector.size())
            exit (0);

        string filename = bodyHeadVector[0].substr(bodyHeadVector[0].find("\"") + 1);

        filename.erase(filename.find("\""));

        client._requestString.erase(0, pos_1);
        pos = client._requestString.find(boundary);
        tmp = client._requestString.substr(0, pos);


        client.getRequest().setBody(tmp);
        tmp.clear();
    }

}

int SocketClass::communicate(struct pollfd &fds)
{

    cerr << "communicate" << endl;
    string			req;
    size_t			size;
    char			request[MAX_REQUEST_SIZE] = {0};

    size = recv(fds.fd, request, MAX_REQUEST_SIZE, 0);
    if ( !recvError(size, fds.fd) ) return 0;

    if (_clients.find(fds.fd) == _clients.end())
    {
        Client client(fds.fd);
        _clients.insert(make_pair(fds.fd, client));
    }

    for(size_t i = 0; i < size; i++) _clients[fds.fd]._requestString.push_back(request[i]);

    _clients[fds.fd].receiveHeader();

    if (_clients[fds.fd].getStatus() == HEADER_RECEIVED)
    {
        httpRequest = Request::deserialize(_clients[fds.fd].getHeader());

        int isOk = httpRequest.isReqWellFormed();
        if (isOk != 200)
        {
            cerr << "Request not well formed >> " << isOk << "<<" << endl;
            exit(0);
        }

        if (_clients[fds.fd].isHeaderRecived())
            _clients[fds.fd].setRequest(httpRequest); // set request without body

        if (_clients[fds.fd].getRequest().getMethod() == POST)
        {
            handlePostRequest(_clients[fds.fd]);
        }
        else if (_clients[fds.fd].getRequest().getMethod() == GET)
        {
            _clients[fds.fd].setStatus(FILE_NOT_SET);
            fds.events = POLLOUT;
        }
        else if (_clients[fds.fd].getRequest().getMethod() == DELETE)
        {
            cerr << "************************DELETE method" << endl;
            exit(0);
        }
    }


    return true;
}


bool SocketClass::isNewConnection(int listener)
{
    cerr << "NewConnection" << endl;
    for (size_t i = 0; i < _s.size(); i++)
    {
        if (_s[i].sockfd == listener)
            return true;
    }
    return false;
}

struct pollfd SocketClass::createPollfd(int sockfd)
{
    struct pollfd pfd;
    pfd.fd = sockfd;
    pfd.events = POLLIN;
    return pfd;
}

void SocketClass::setFds()
{
    int listener;
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        listener = this->create();
        
        _s.push_back(SocketServer(listener, it->getPort(), it->getHost()));

        this->bindSocket(listener, _s[_s.size() - 1]);
        this->listenSocket(listener);
        _fds.push_back(createPollfd(listener));

    }

    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        cerr << "http://localhost:" << it->getPort() << endl;
    }
}

void SocketClass::run()
{
    int     current_size;
    int     newClient;

    setFds();
    
    while (true)
    {
        // cout << "listen ..." << endl;
        if (poll(&_fds[0], _fds.size(), -1) < 0) break;

        current_size = _fds.size();
        for(int i = 0; i < current_size; i++)
        {
            if (_fds[i].revents == 0)
                continue;
            if (_fds[i].revents & POLLHUP)
            {
                close(_fds[i].fd);
                _clients.erase(_fds[i].fd);
                _fds.erase(_fds.begin() + i);
            }
            else if (_fds[i].revents & POLLIN)
            {   
                if (isNewConnection(_fds[i].fd))
                {
                    newClient = accept(_s[i].sockfd, (struct sockaddr*)&_s[i].address, (socklen_t*)&_s[i].addrlen);
                    if (newClient < 0) { cerr << "fail to accept connection" << '\n'; return ; }

                    _fds.push_back(createPollfd(newClient));
                }
                else if ( !communicate(_fds[i]) )
                    break;
            }

            if (_fds[i].revents & POLLOUT)
            {
                httpRequest.resource();
                
                string file = joinRootAndPath(LOCALHOST, httpRequest.getRessource(), httpRequest);
                
                sendFileInPackets(file, _fds[i]);
                
                if (_clients[_fds[i].fd].getStatus() == SENDED)
                {
                    close(_fds[i].fd);
                    _clients.erase(_fds[i].fd);
                    _fds.erase(_fds.begin() + i);
                }
                
            }
        }

    }

}

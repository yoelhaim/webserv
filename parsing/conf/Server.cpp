/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoelhaim <yoelhaim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 21:13:42 by yoelhaim          #+#    #+#             */
/*   Updated: 2023/04/19 00:44:05 by yoelhaim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Location.hpp"


Server::Server(directives dir) : Http(dir)
{
    this->_server_name = dir.server_name;
    this->_listen = dir.listen;
    this->_host = dir.host;
    this->_matched_location = "/";
    this->_cgi_info_php = dir.cgi_info_php;
    this->_cgi_info_py = dir.cgi_info_py;
    
}

Server::~Server() {}

Server::Server(const Server &copy) :Http(copy)
{
    *this = copy;
}

Server &Server::operator=(const Server &copy)
{
    if (this != &copy)
    {
        this->_server_name = copy._server_name;
        this->_listen = copy._listen;
        this->_locations = copy._locations;
        this->_host = copy._host;
        this->_matched_location = copy._matched_location;
        this->_cgi_info_php = copy._cgi_info_php;
        this->_cgi_info_py = copy._cgi_info_py;
        _locations = copy._locations;
    }

    return *this;
}

void Server::setMatchedLocation(string location)
{
    this->_matched_location = location;
}

string Server::getServerName() const
{
    return this->_server_name;
}

string Server::getHost() const
{
    return this->_host;
}

vector<short int> Server::getPort() const { return this->_listen; }


void Server::setLocation(string path, Location locations)
{
    if (this->_locations.find(path) != this->_locations.end())
    {
        cerr << "Error: duplicate location path" << endl;
        exit(1);
    }
    this->_locations[path] = locations;
}

size_t Server::getLengthLocation() const
{
    return this->_locations.size();
}

string Server::getMatchedLocation() const
{
    return this->_matched_location;
}

string Server::getCgiInfoPHP() const
{
    return this->_cgi_info_php;
}
string Server::getCgiInfoPY() const
{
    return this->_cgi_info_py;
}

void Server::setPort(short int port)
{
    this->_listen.push_back(port);
}


string Server::getUploadPath(string matchLocation) 
{
    map<string, string> map = _locations[matchLocation]._directives;
    if (map.find("root") != map.end())
        return map["root"];
      
    return this->_root;
}
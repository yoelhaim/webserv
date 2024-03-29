/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matef <matef@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/24 00:00:05 by yel-khad          #+#    #+#             */
/*   Updated: 2023/04/19 02:34:48 by matef            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "get.hpp"

Get::Get(Request request, Server server) : Method(request, server)
{
    if (!getRequestedResource())
    {
        _status = 404;
        _comment = "Not Found";
        _resp = getFileContent(_error_page[404]);
        return ;
    }
    if (isFile())
    {
        if (!hasCGI())
        {
            _status = 200;
            _comment = "OK";
            _resp = getFileContent(_url);
            _contentType = _mime.getMimeType(_mime.getExtension(_url));
            return ;
        }
        CGI cgi(request, server, _url, "GET");
        _resp = cgi.getResp();
        if (_resp == "error")
        {
            _resp = getFileContent(_error_page[502]);
            _status = 502;
            _comment = "Bad Gateway";
            return ;
        }
        deserialize();
        _resp = getRidOfHeaders();
        _status = 200;
        _comment = "OK";
        return ;
    }
    
    if (!hasSlashInTheEnd())
    {
        _status = 301;
        _comment = "Moved Permanently";
        _resp = getFileContent(_error_page[301]);
        _contentType = _mime.getMimeType(_mime.getExtension(_error_page[301]));
       
        return ;
    }
    if (!hasIndexFile())
    {
        
        string file = _url + "index.html";
        ifstream f(file);
        if(f.is_open())
        {
            f.close();
            _resp = getFileContent(file);
            _contentType = _mime.getMimeType(_mime.getExtension("./index.html"));
            _status = 200;
            _comment = "OK";
            return ;
        }
        
        if (!getAutoIndex())
        {
            _status = 403;
            _comment = "Forbidden";
            _resp = getFileContent(_error_page[403]);
            _contentType = _mime.getMimeType(_mime.getExtension(getFileContent(_error_page[403])));
            return ;
        }
        Worker::listenDirectory(_url, server.getMatchedLocation());
        _status = 200;
        _comment = "OK";
        _resp = getFileContent("./configuration/dir/index.html");
        _contentType = _mime.getMimeType(_mime.getExtension("./configuration/dir/index.html"));
        return;
    }
    _url = getIndex();
    if (_url.empty())
    {
        _status = 404;
        _comment = "Not Found";
        _resp =getFileContent(_error_page[404]);
        _contentType = _mime.getMimeType(_mime.getExtension(getFileContent(_error_page[404])));
        return ;
    }
    if (!hasCGI())
    {
        
        _status = 200;
        _comment = "OK";
        _resp = getFileContent(_url);
        _contentType = _mime.getMimeType(_mime.getExtension(_url));
        return ;
    }
    CGI cgi(request, server, _url, "GET");
    _resp = cgi.getResp();

    if (_resp == "error")
    {
        _resp = "";
        _status = 502;
        _comment = "Bad Gateway";
        return ;
    }
    deserialize();
    _resp = getRidOfHeaders();
    _status = 200;
    _comment = "OK";
    return ;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-khad <yel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/24 00:00:05 by yel-khad          #+#    #+#             */
/*   Updated: 2023/03/26 07:15:35 by yel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "get.hpp"

Get::Get(Request request, Server server) : Method(request, server)
{
    if (!getRequestedResource())
    {
        _status = 404;
        _comment = "Not Found";
        _path = "./error_page/404.html";
        return ;
    }
    if (isFile())
    {
        if (!hasCGI())
        {
            _status = 200;
            _comment = "OK";
            _path = _url;
            return ;
        }
        // RUN CGI ON REQUESTED FILE
        return ;
    }
    if (!hasSlashInTheEnd())
    {
        _status = 301;
        _comment = "Moved Permanently";
        _path = "./error_page/301.html";
        return ;
    }
    if (!hasIndexFile())
    {
        if(FILE *f = fopen("index.html","r"))
        {
            fclose(f);
            _path = "index.html";
            _status = 200;
            _comment = "OK";
            return ;
        }
        if (!getAutoIndex())
        {
            _status = 403;
            _comment = "Forbidden";
            _path = "./error_page/403.html";
            return ;
        }
        Worker::listenDirectory(_url);
        _status = 200;
        _comment = "OK";
        _path = "./dir.html";
        return;
    }
    _url = getIndex();
    if (_url.empty())
    {
        _status = 403;
        _comment = "Forbidden";
        _path = "./error_page/403.html";
        return ;
    }
    if (!hasCGI())
    {
        _status = 200;
        _comment = "OK";
        _path = _url;
        return ;
    }
    // RUN CGI ON REQUESTED FILE
}
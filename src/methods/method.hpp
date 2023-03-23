/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-khad <yel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 18:25:16 by yel-khad          #+#    #+#             */
/*   Updated: 2023/03/23 20:45:30 by yel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef METHOD_HPP
# define METHOD_HPP

# include <iostream>
# include <dirent.h>
# include <stdio.h>
# include "../../parsing/request/Request.hpp"
# include "../../parsing/conf/Server.hpp"
# include "../../parsing/conf/Location.hpp"


using namespace std;

class Method 
{
    protected:
        Request _request;
        Server  _server;
        string _path;
        map<string, Header> headers;
    public:
        Method(Request request, Server server);
        bool getRequestedResource();
        bool isFile();
        bool hasSlashInTheEnd();
        bool hasIndexFile();
        bool getAutoIndex();
        bool hasCGI();
};

class Post : public Method
{
    public:
        Post(Request request, Server server);
        bool locationSupportUpload();
};

class Delete : public Method
{
    public:
        Delete(Request request, Server server);
        bool hasWriteAccesOnFolder();
          
};

#endif

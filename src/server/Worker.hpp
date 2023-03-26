/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Worker.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-khad <yel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 22:36:44 by yoelhaim          #+#    #+#             */
/*   Updated: 2023/03/26 02:16:55 by yel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <dirent.h>
#include <fstream>

#include "../../parsing/conf/Server.hpp"
#include "../../parsing/conf/Location.hpp"
#include  "../../parsing/request/Request.hpp"
#include "../methods/method.hpp"
#include "../methods/delete.hpp"



class Worker
{
private:
    
public:
    Worker();
    ~Worker();
    Worker(const Worker &copy);
    Worker &operator=(const Worker &copy);
    bool getMatchedLocationFoRequestUri(string requestUri,  Server &servers);
    bool isLocationHaveRedirection(Server &servers);
    bool isMethodAllowdedInLocation(string Method,Server &servers);
    bool getAllowdedMethods(string methods, string allow);
    Method getMethodObject(Request &req, Server &server);
    void  runMethodGet(Request &req, Server &server);
    void  runMethodPost(Request &req, Server &server);
    Delete  runMethodDelete(Request &req, Server &server);
    static string listenDirectory(string path);
    
    
};


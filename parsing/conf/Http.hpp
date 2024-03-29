/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoelhaim <yoelhaim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 00:23:47 by yoelhaim          #+#    #+#             */
/*   Updated: 2023/04/18 21:12:47 by yoelhaim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <vector>
using namespace std;

#define ROOTPAGE "./var/www/html"

struct directives
{
    size_t cli_max_size ;
    vector<short int> listen ;
    string server_name ;
    string host ;
    string root ;
    string index ;
    map<int, string > error_page;
    bool autoindex;
    string allow;
    string cgi_info_php;
    string cgi_info_py;
    

    directives()
    {
        cli_max_size = 1000;
        server_name = "localhost";
        host  = "127.0.0.1";
        root = "./www/html";
        autoindex = false;
        allow = "GET POST DELETE";
    }
};


enum e_token
{
    OPEN_CURLY,
    CLOSE_CURLY,
    SEMI_COLON,
    WORD,
    DIRECTIVE,
    CONTEXT,
    WSPACE,
    NEWLINE,
   
};

class Http
{
protected:
    size_t _cli_max_size;
    string _root;
    string _index;
    map<int, string > _error_page;
    string _allowed_methods;
    bool _autoindex;

public:
    Http(directives dir = directives());
    Http(const Http &copy);
    Http &operator=(const Http &copy);
    ~Http();

    size_t getMaxSize() const;
    string getRoot() const;
    string getIndex() const;
    map<int, string > getErrorPage() const;
    string getErrorPageByIndex(size_t index) const;
    string getAllowedMethods(int index) const;
    bool getAutoIndex() const;

};
// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#include "HttpServer.h"
#include "HttpConnection.h"

namespace sealedserver
{

HttpConnection::HttpConnection(HttpServer *server, EventLoop *loop, Socket sock) : \
	MSGHandler(loop, sock), server_(server), errcode_(0)
{
}

HttpConnection::~HttpConnection()
{
}

void HttpConnection::receivedMsg(STATUS status, Buffer &receivedBuff)
{
	parse(receivedBuff);
	
	if(errcode_ != 0) badRequest();
	else server_ -> process(this);

	//unRegisterRead();
}

void HttpConnection::sentMsg(STATUS status, int len, int targetLen)
{
	close();
}

static char *skip(char **buf, const char *delimiters) {
  char *p, *begin_word, *end_word, *end_delimiters;

  begin_word = *buf;
  end_word = begin_word + strcspn(begin_word, delimiters);
  end_delimiters = end_word + strspn(end_word, delimiters);

  for (p = end_word; p < end_delimiters; p++) {
    *p = '\0';
  }

  *buf = end_delimiters;

  return begin_word;
}

void HttpConnection::parse(Buffer &receivedBuff)
{
	string str = (string)receivedBuff;

	int index; bool flag;
	flag = parseFirstLine(str, index);

	if(flag == false){
		errcode_ = 1;
		return;
	} 

	flag = check();
	if(flag == false){
		errcode_ = 1;
		return;
	} 

	flag = parseURL();
	if(flag == false){
		errcode_ = 1;
		return;
	} 

	str  = str.substr(index + 2);
	flag = parseHeader(str);

	if(flag == false) 
		errcode_ = 1;
}

bool HttpConnection::parseFirstLine(const string &str, int &index)
{
	int i = 0, len = str.size(), j;
	
	/// skip space
	for(;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;
	
	/// Find the first space
  	for(j = i + 1;j < len && str.at(j) != ' ';j++);
  	if(j >= len) return false;

  	method_ = str.substr(i, j - i);

  	/// skip space
  	for(i = j;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;
	
	/// Find the first space
  	for(j = i + 1;j < len && str.at(j) != ' ';j++);
  	if(j >= len) return false;

  	url_ = str.substr(i, j - i);

  	/// skip space
  	for(i = j;i < len && str.at(i) == ' ';i++);
	if(i >= len) return false;

	/// Find the `\r` space
 	for(j = i + 1;j < len && str.at(j) != '\r';j++);
 	if(j >= len - 1 || str.at(j + 1) != '\n') return false;
	
  	version_ = str.substr(i, j - i);	

  	index = j + 2;

  	return true;
}

bool HttpConnection::parseHeader(string str)
{
	bool flag = true;
	while(!errcode_ && str.size() != 0)
	{
		int index = str.find("\r\n");
		if(index == -1) {
			return false;
		} 

		if(index == 0) break;
		
		flag = parseLine(str.substr(0, index));
		
		if(flag == false){
			return false;
		}

		str = str.substr(index + 2);
	}

	return true;
}

bool HttpConnection::parseLine(string str)
{	
	int index = str.find(": ");
	
	if(index == -1 || index == 0 || index >= str.size() - 2)
		return false;

	header_[str.substr(0,index)] = str.substr(index + 2);
}

/// only support get
int HttpConnection::is_valid_http_method(const char *s) {
  return !strcmp(s, "GET");
}

bool HttpConnection::check()
{
	if(is_valid_http_method(method_.c_str()) == false) 
		return false;

	if (memcmp(version_.c_str(), "HTTP/", 5) != 0)
		return false;

	return true;
}

bool HttpConnection::parseURL()
{
	int index = url_.find("?");
	bool flag = true;

	if(index == -1)
		querystring_ = url_;
	else
	{
		querystring_ = url_.substr(0, index + 1);
		flag = decode(url_.substr(index + 1));
	}

	return flag;
}

#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

bool HttpConnection::decode(string para)
{
	int i, a, b; string decodestr;
	int len = para.size();
		
	for(i = 0; i < len;i++)
	{
		if(para.at(i) == '%' && i < para.size() -2 &&
			isxdigit(para.at(i+1)) && isxdigit(para.at(i+2)))
		{
			a = tolower(para.at(i + 1));
			b = tolower(para.at(i + 2));
			decodestr.push_back((char) ((HEXTOI(a) << 4) | HEXTOI(b)));
			i += 2;
		}
		else if(para.at(i) == '+'){
			decodestr.push_back(' ');
		}
		else{
			decodestr.push_back(para.at(i));
		}
	}

	bool flag = true;

	while(flag == true)
	{
		int index = decodestr.find("&");
		if(index == -1)
		{
			flag = parsekv(decodestr);
			break;
		}
		flag = parsekv(decodestr.substr(0, index));
		decodestr = decodestr.substr(index + 1);
	}

	return flag;
}

bool HttpConnection::parsekv(string str)
{
	int index = str.find("=");
	if(index == -1 || index == 0)
		return false;
	params_[str.substr(0, index)] = str.substr(index + 1);

	return true;
}

static const char *status_code_to_str(int status_code) {
  switch (status_code) {
    case 200: return "OK";
    case 201: return "Created";
    case 204: return "No Content";
    case 301: return "Moved Permanently";
    case 302: return "Found";
    case 304: return "Not Modified";
    case 400: return "Bad Request";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 409: return "Conflict";
    case 411: return "Length Required";
    case 413: return "Request Entity Too Large";
    case 415: return "Unsupported Media Type";
    case 423: return "Locked";
    case 500: return "Server Error";
    case 501: return "Not Implemented";
    default:  return "Server Error";
  }
}

void HttpConnection::notFound()
{
	/// TBD
	char buf[100];
	memset(buf, 0, 100);
	sprintf(buf, "HTTP/1.1 %d %s\r\n\r\n", 404, status_code_to_str(404));

	write(buf);
}

void HttpConnection::badRequest()
{
	/// TBD
	char buf[100];
	memset(buf, 0, 100);
	sprintf(buf, "HTTP/1.1 %d %s\r\n\r\n", 400, status_code_to_str(400));

	write(buf);
}


string HttpConnection::getQuery() const
{
	return querystring_;
}

};
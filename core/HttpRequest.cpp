// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace sealedserver
{

HttpRequest::HttpRequest(HttpServer *server, EventLoop *loop, Socket sock) : \
	MSGHandler(loop, sock), server_(server), errcode_(0), first_(false)
{
}

HttpRequest::~HttpRequest()
{
	if(response_)
		delete response_;
	
	response_ = NULL;
}

void HttpRequest::received(STATUS status, Buffer &receivedBuff)
{
	if(first_ == true) return;

	first_ = true;

	parse(receivedBuff);

	if(errcode_ != 0) {
		initResponse(400);
		badRequest();
	}else server_ -> process(this);
}

void HttpRequest::sent(STATUS status, int len, int targetLen)
{
	close();
}

void HttpRequest::parse(Buffer &receivedBuff)
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

bool HttpRequest::parseFirstLine(const string &str, int &index)
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

bool HttpRequest::parseHeader(string str)
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

bool HttpRequest::parseLine(string str)
{	
	int index = str.find(": ");
	
	if(index == -1 || index == 0 || index >= str.size() - 2)
		return false;

	header_[str.substr(0,index)] = str.substr(index + 2);
}

/// only support get
int HttpRequest::is_valid_http_method(const char *s) {
  return !strcmp(s, "GET");
}

bool HttpRequest::check()
{
	if(is_valid_http_method(method_.c_str()) == false) 
		return false;

	if (memcmp(version_.c_str(), "HTTP/", 5) != 0)
		return false;

	return true;
}

bool HttpRequest::parseURL()
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

bool HttpRequest::decode(string para)
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

bool HttpRequest::parsekv(string str)
{
	int index = str.find("=");
	if(index == -1 || index == 0)
		return false;
	params_[str.substr(0, index)] = str.substr(index + 1);

	return true;
}

/// Not found & without error callback
void HttpRequest::notFound()
{
	response_ -> addHeader("Content-Type", "text/html");

	response_ -> addBody("<HTML><TITLE>Not Found</TITLE>");
	response_ -> addBody("<BODY><P>The server could not fulfill")
	response_ -> addBody("your request because the resource specified");
	response_ -> addBody("is unavailable or nonexistent.");
	response_ -> addBody("</BODY></HTML>");

	response_ -> send();
}

void HttpRequest::badRequest()
{
	response_ -> addBody("<HTML><TITLE>Bad Request</TITLE>");
	
	response_ -> addBody("<BODY><P>The server could not fulfill")
	response_ -> addBody("your request because the resource specified");
	response_ -> addBody("is unavailable or nonexistent.");
	response_ -> addBody("</BODY></HTML>");

	response_ -> send();
}


string HttpRequest::getQuery() const
{
	return querystring_;
}

string HttpRequest::initResponse(int code)
{
	response_ = new HttpRequest(this, code);
}

};
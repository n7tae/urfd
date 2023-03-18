/*
 *   Copyright (c) 2023 by Thomas A. Early N7TAE
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#pragma once

#include <curl/curl.h>
#include <iostream>
#include <string>

class CCurlGet
{
public:
	CCurlGet();
	~CCurlGet();
	// the contents of the URL will be appended to the stringstream.
	CURLcode GetURL(const std::string &url, std::stringstream &ss, long timeout = 30);
private:
	static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp);
};

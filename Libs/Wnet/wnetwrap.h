/*
-------------------------
----------------TO-DO----
-------------------------
--------sessions
todo: preserve session until UA or proxy change - dont use InternetOpen again unless params have been changed
DONE: but session is never closed - does it automatically get closed when return 0 is called at the end of main? create global class with destructor?
todo: allow multiple sessions/connections https://stackoverflow.com/questions/48651185/why-two-calls-to-httpsendrequest-on-different-connections-new-internetopen-and#49061318

---------proxy 
todo: error catching for setting proxy usr/pwd
todo: function for user to clear proxy (reason for creating session obj?)
todo: encode proxy usr/pwd - is this needed or does WinINet do it? (test + check)
todo: see option flags INTERNET_OPTION_PER_CONNECTION_OPTION "To change proxy information for the entire process without affecting the global settings..."
https://docs.microsoft.com/en-us/windows/win32/wininet/option-flags

-------cookies
todo: set-cookies recd must take into account other = than name <----- TEST

--------redirects
todo: when redirects are off, allow it to pull all info, status code headers etc
todo: redirect adjust method e.g. POST to GET https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Location

---------testing needed:
test proxy
test abs redirect
test cookies with more attributes data than just cookie name=value
test cookies with encoding off

todo: accept gzip data... see this async wrapper that does it: https://www.codeproject.com/articles/43860/an-asynchronous-http-download-class-for-mfc-atl-an



-------------------------
----------------NOTES----
-------------------------
- cookies only set for url without path (domain only) unless domain or path is set in cookie data
- session cookies not preserved

- url encoding of space is + not %20

- HttpOpenRequestA and HttpSendRequestA seem to work passing utf-8 params even though the docs recommend always using the W commands
- in url only query params, not host / path is url encoded 
However docs suggest host is auto encoded: "By default, the host or authority portion of the URL is encoded according to the IDN specification for both direct and proxy connections. "
https://docs.microsoft.com/en-us/windows/win32/wininet/option-flags -> INTERNET_OPTION_IDN

fixed: post data size limit - fixed by converting postdata string to char array before sending
*/
#define WIN32_LEAN_AND_MEAN
#pragma once

#include <string>
#include <windows.h>
#include <WinInet.h>
#include <Winineti.h>
#include <stdio.h>
#include <map>
#include <initializer_list>
#include <cctype>
#include <stdlib.h>
#include <urlmon.h>


namespace wrap {



	struct Comparator {
		bool operator() (const std::string& s1, const std::string& s2) const {
			std::string str1;
			std::string str2;
			//std::transform(s1.begin(), s1.end(), str1.begin(), tolower);
			for (auto& c : s1)
			{
				str1 += std::tolower(c);
			}
			for (auto& c : s2)
			{
				str2 += std::tolower(c);
			}

			//std::transform(s2.begin(), s2.end(), str2.begin(), tolower);
			return  str1 < str2;
		}
	};

	struct Body { //used for postdata
		std::string body;
	};

	struct File { //used for uploading file multipart post
		std::string file;
	};

	struct Download {
		std::string dl = "dl"; //default is to download file keeping original filename
	};

	struct Method {
		std::string method;
	};

	struct Url {
		std::string adr;
	};

	struct UserAgent {
		std::string usr;
	};

	struct Header {
		std::map<std::string, std::string> hdr;

		Header(std::initializer_list<std::pair<const std::string, std::string> > hdr) :
			hdr(hdr) {
		}
	};

	struct Parameters {
		std::map<std::string, std::string> p;

		Parameters(std::initializer_list<std::pair<const std::string, std::string> > p) :
			p(p) {
		}
	};

	struct Cookies {
		std::map<std::string, std::string> c;
		Cookies(std::initializer_list<std::pair<const std::string, std::string> > c ) :
			c(c) {
		}
	};

	struct Payload {
		std::map<std::string, std::string> pd;

		Payload(std::initializer_list<std::pair<const std::string, std::string> > pd) :
			pd(pd) {
		}
	};

	struct Multipart {
		std::map<std::string, std::string> mp;

		Multipart(std::initializer_list<std::pair<const std::string, std::string> > mp) :
			mp(mp) {
		}
	}; 

	struct Authentication {
		std::string usr;
		std::string pwd;
	};

	struct Bearer {
		std::string token;
	};

	struct Timeout {
		DWORD timeout;
		std::string type = "request"; // connection or request
	};

	struct Options {
		std::map<std::string, std::string> options;
		Options(std::initializer_list<std::pair<const std::string, std::string> > options) :
			options(options) {
		}
	};

	struct Proxy {
		std::string url;
		std::string usr = "";
		std::string pwd = "";
	};

	struct req {

		std::string ua = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
		std::string Params;
		std::string PostData; // key is type of data (body,payload or file) and value is the data
		std::string Dl;
		std::string Method = "GET";
		std::string Auth;
		std::string Url = "www.example.com";
		wrap::Proxy Proxy;
		std::map <std::string, std::string> cookies_tosend;
		bool cookie_encode = true;
		bool redirect = true;
		bool newsession = true; //if current session should be replaced with a new one via InternetOpen - triggered by proxy or user agent changes
		HINTERNET session_handle = NULL;
		Header Header = { {"",""} };
		DWORD TimeoutConnect = 0;
		DWORD TimeoutRequest = 0;
		DWORD AccessType = INTERNET_OPEN_TYPE_DIRECT;
		void reset() {
			ua = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
			Params = "";
			PostData = "";
			Dl = "";
			Method = "GET";
			Auth = "";
			Url = "www.example.com";
			Header = { {"",""} };
			//cookies_tosend.clear(); - currently done at end of prepare_cookies()
			TimeoutConnect = 0;
			TimeoutRequest = 0;
			newsession = false;
			cookie_encode = true;
			redirect = true;

		};
	};



	struct Response {
		std::map <std::string, std::string, Comparator> header;
		std::map <std::string, std::string> sent_headers;
		std::map <std::string, std::string> secinfo;
		std::map <std::string, std::string> cookies;
		std::string url;
		std::string raw;
		std::string text;
		std::string status_code;
		std::string err;
		unsigned long uploaded_bytes = 0;
		unsigned long downloaded_bytes = 0;
		unsigned long redirect_count = 0;
	};



	std::string text_from_html(std::string html);

	//initial req object that will be used to pass params to cpp source function
	extern req toSource;

	void Params(wrap::Parameters s);

	void Params(wrap::Url u);

	void Params(wrap::UserAgent usr);

	void Params(wrap::Header h);

	void Params(wrap::Payload pd);

	void Params(wrap::Multipart mp);

	void Params(wrap::Authentication auth);

	void Params(wrap::Bearer token);

	void Params(wrap::Method m);

	void Params(wrap::Download dl);

	void Params(wrap::Body body);

	void Params(wrap::Cookies c);

	void Params(wrap::File file);

	void Params(wrap::Timeout timeout);

	void Params(wrap::Options options);

	void Params(wrap::Proxy proxy);

	// url, headers, params, postdata, cookies, dl flag, proxies, timeout, method, auth 
	Response httpsreq(req Request);

	//this is to achieve the effect of allowing random params of different types
	//https://stackoverflow.com/questions/67089840
	//not even URL is needed - it's set to example.com by default
	template <typename ...Ts>
	Response HttpsRequest(Ts&& ...args)
	{

		std::initializer_list<int> ignore = { (Params(args), 0)... };
		(void)ignore; // Hack that prevents the initializer_list from being optimized and the "ignore variable is unused" warning
		return httpsreq(toSource);
	};


}
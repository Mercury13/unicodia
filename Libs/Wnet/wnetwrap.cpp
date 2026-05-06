#include "wnetwrap.h"

#include <cstdint>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <fstream>

#include "mojibake.h"

struct TPARAMS {
	HINTERNET hInternet = NULL;
	HINTERNET hInternetOut = NULL; //returned handle
	std::string host = "";
	INTERNET_PORT port = 0;
	DWORD service = 0;
};
struct TRPARAMS { 
	HINTERNET hRequest = NULL;
    BOOL res = 0;
	std::string headers = "";
};

struct UriInfo {
    std::string scheme, host, path, urlfile;
};

//-----------------------UTILITY DECLARATIONS
static inline void ltrim(std::string& s); //whitespace removal
//static inline void rtrim(std::string& s);
//static inline void trim(std::string& s);
std::string GetMimeType(const std::string& ext); //used to set content-type for multipart file upload
std::string url_encode(std::string const& str);
std::string hex_encode(char const c);
static std::string base64_encode(const std::string& in); //used for basic auth

//----------------------HTTP REQ DECLARATIONS
static inline UriInfo handleURI(wrap::req& request, wrap::Response& output, INTERNET_PORT& port);
static inline bool TimeoutInternetConnect(HINTERNET& hInternet, HINTERNET& hConnect, wrap::Response& output, std::string host, INTERNET_PORT port, DWORD service);
static inline bool TimeoutInternetRequest(HINTERNET& hRequest, std::string final_headers, wrap::Response& output);
static inline std::string prepare_headers(wrap::req& request, const UriInfo& URI);
static inline std::string get_response(HINTERNET& hRequest, wrap::Response& output, wrap::req& request);
static inline std::string get_statuscode_redirect(HINTERNET& hRequest, wrap::Response& output);
static inline void get_recd_headers(HINTERNET& hRequest, wrap::req& request, wrap::Response& output, std::string& redirect, UriInfo& URI);
static inline void get_sent_headers(HINTERNET hRequest, wrap::Response& output);
static inline void get_secinfo(HINTERNET hRequest, wrap::Response& output); //get all security info and put it into response obj
DWORD WINAPI WorkerInternetConnect(LPVOID); //worker thread functions for timeouts
DWORD WINAPI WorkerInternetRequest(LPVOID);

//---------------------OVERLOADED PARAMS DECLARATIONS
void wrap::Params(wrap::Timeout timeout);
void wrap::Params(wrap::Bearer token);
void wrap::Params(wrap::Parameters p);
void wrap::Params(wrap::Url u);
void wrap::Params(wrap::Header h);
void wrap::Params(wrap::Cookies c);
void wrap::Params(wrap::Method m);
void wrap::Params(wrap::Download dl);
void wrap::Params(wrap::File f);
void wrap::Params(wrap::Body b);
void wrap::Params(wrap::Payload pd);
void wrap::Params(wrap::Multipart mp);
void wrap::Params(wrap::Authentication auth);

wrap::req wrap::toSource;


wrap::Response wrap::httpsreq(wrap::req request) {
	wrap::Response output;

	if (wrap::toSource.newsession) {
		if (wrap::toSource.session_handle != NULL) { InternetCloseHandle(wrap::toSource.session_handle); }
		wrap::toSource.session_handle = InternetOpenA(wrap::toSource.ua.c_str(), wrap::toSource.AccessType, wrap::toSource.Proxy.url.c_str(), NULL, 0);
		if ((wrap::toSource.session_handle != NULL) && (wrap::toSource.Proxy.url != "") && (wrap::toSource.Proxy.usr != "")) { 
			InternetSetOptionA(wrap::toSource.session_handle,INTERNET_OPTION_PROXY_USERNAME, &wrap::toSource.Proxy.usr, wrap::toSource.Proxy.usr.length());
			InternetSetOptionA(wrap::toSource.session_handle, INTERNET_OPTION_PROXY_PASSWORD, &wrap::toSource.Proxy.pwd, wrap::toSource.Proxy.pwd.length());
		}
		wrap::toSource.newsession = false;
	}

	if (wrap::toSource.session_handle == NULL)
	{
        output.err = "InternetOpen failed: " + std::to_string(GetLastError());
		return output;
	}

	internetopened: //used for absolute URI redirect where both path and host have changed

	DWORD service = INTERNET_SERVICE_HTTP;
	INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;
    UriInfo URI = handleURI(request, output, port); //get host (for InternetConnect) from path (used in HttpOpenRequest) and get protocol

	HINTERNET hConnect = NULL;
	if (wrap::toSource.TimeoutConnect == 0) { //if no timeout set then just do normal call
        hConnect = InternetConnectA(wrap::toSource.session_handle, URI.host.c_str(), port, nullptr, nullptr, service, 0, 0);
	} else { //if timeout set then use threads due to MS bug https://mskb.pkisolutions.com/kb/224318
        TimeoutInternetConnect(wrap::toSource.session_handle, hConnect, output, URI.host, port, service);
	}
	if (hConnect == NULL)
	{
        output.err = "InternetConnect failed: " + std::to_string(GetLastError());
		return output;
	}

	connectionopened: //used for relative URI redirect where only path (not host) changes
    HINTERNET hRequest = HttpOpenRequestA(hConnect, request.Method.c_str(), URI.path.c_str(), NULL , NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_AUTO_REDIRECT, 0);

	if (hRequest == NULL)
	{
        output.err = "HttpOpenRequest failed: " + std::to_string(GetLastError());
		return output;
	}
	
	std::string final_headers = prepare_headers(request, URI);

    BOOL sendr = 0u;
	if (wrap::toSource.TimeoutRequest == 0) { //if no timeout set then just do normal call 
		//sending data as bytes (c string) to ensure files e.g. images or zip files get transfered ok - see here: https://stackoverflow.com/a/16502000/13666347
		sendr = HttpSendRequestA(hRequest, final_headers.c_str(), -1L, &wrap::toSource.PostData[0], sizeof(char) * wrap::toSource.PostData.size());
	}
	else { //if timeout set then use threads due to MS bug https://mskb.pkisolutions.com/kb/224318
		sendr = TimeoutInternetRequest(hRequest, final_headers, output);
	}
	if (!sendr)
	{
        output.err = "HttpSendRequest failed with error code " + std::to_string(GetLastError());
		return output;
	}

	std::string strResponse = get_response(hRequest, output, request);
	std::string redirect = get_statuscode_redirect(hRequest, output); //store status code and check if its a redirect
	get_recd_headers(hRequest, request, output, redirect, URI);
	if (wrap::toSource.redirect) {//if redirects are turned on
		//if this is a redirect, close handles and redo request for new URL
		if (redirect == "absolute") {
			//std::cout << "absredir" << std::endl;
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnect);
			goto internetopened;
		}
		if (redirect == "relative") { //host stays the same so only redo httpopenrequest and sendrequest
			//std::cout << "relredir" << std::endl;
			InternetCloseHandle(hRequest);
			goto connectionopened;
		}
	}
	get_sent_headers(hRequest, output); //populate output.sent_headers
	get_secinfo(hRequest, output); // populate output obj with all security info

	output.raw = strResponse;
	std::string doctype = strResponse.substr(0, 14);
	for (auto& c : doctype) {c = std::tolower(c);} //get lowercase of first 14 chars and check if its html
	if (doctype=="<!doctype html") {
		output.text = wrap::text_from_html(strResponse); // if html, do some very basic parsing to get the text
	} else { output.text = output.raw; //if no html found, .text = .raw		
	}

	wrap::toSource.reset(); //return request object back to defaults - this gets rid of params etc
	
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	//InternetCloseHandle(wrap::toSource.session_handle);//std::cout << "end of httprequest" << std::endl;
return output;	
}

template <typename... Ts>
wrap::Response wrap::HttpsRequest(Ts&& ...args);




///////////////////////////////////////////////////////////////////////
///////////////////////////// HttpRequest Functions ///////////////////
///////////////////////////////////////////////////////////////////////

static inline UriInfo handleURI(wrap::req& request, wrap::Response& output, INTERNET_PORT& port) {
	//do some very basic URI parsing to separate host (for InternetConnect) from path (used in HttpOpenRequest)
	//also to see what protocol is specified
    UriInfo URI;
	//scheme and host
    URI.host = request.Url;
    if (auto pColonSlash = URI.host.find("://");
            pColonSlash != std::string::npos) { //get scheme if available
        URI.scheme = URI.host.substr(0, URI.host.find(":"));
        URI.host = URI.host.substr(pColonSlash + 3);
		//cout << "clipped host: " + URI["host"]<< endl;
	}
	else { //otherwise assume it's https 
        URI.scheme = "https";
	}
	//default is https

	//update port
    if (URI.scheme == "https") {
		port = INTERNET_DEFAULT_HTTPS_PORT;
	}
    else if (URI.scheme == "http") {
		port = INTERNET_DEFAULT_HTTP_PORT;
	}
	else {
		output.err = "Error: URL input has scheme other than HTTP/S";
		//return output;
	}

	//path (includes fragments, params etc)
	//assume there is a path if url has a further ? or /
    if ((URI.host.find("/") != std::string::npos) || (URI.host.find("?") != std::string::npos)) {
		//and something actually comes after it - i.e. the ? or / isnt the last char
        if ((URI.host.back() != '?') && (URI.host.back() != '/')) {
            if (auto pSlash = URI.host.find("/"); pSlash != std::string::npos) {
                URI.path = URI.host.substr(pSlash);
                URI.host = URI.host.substr(0, pSlash);
			}
			else {
                auto pQun = URI.host.find("?");
                URI.path = URI.host.substr(pQun);
                URI.host = URI.host.substr(0, pQun);
			}
			//if theres a file extension, store filename
            if (auto pLastSlash = URI.path.find_last_of('/');
                    pLastSlash != std::string::npos) { // dot after last /
                if (URI.path.find('.', pLastSlash + 1) != std::string::npos) {
                    URI.urlfile = URI.path.substr(pLastSlash + 1);
				}
			}
			else {
                if (auto pQun = URI.path.find_last_of('?');
                         pQun != std::string::npos) { // dot after ?
                    if (URI.path.find('.', pQun + 1) != std::string::npos) {
                        URI.urlfile = URI.path.substr(pQun + 1);
					}
				}
			}
		}
		else { //if its the last char, trim because wininet doesnt like a trailing / or ?
            if (!URI.host.empty())
                URI.host.pop_back();
			//std::cout << "trimmed last char of host" << std::endl;
		}
	}

	//add params that were passed as Parameters map
	if (request.Params != "") {
        URI.path += request.Params;
	}

    output.url = URI.scheme + "://" + URI.host + URI.path;

	//entering dl means the file is saved as its original filename
	if (request.Dl == "dl") {
        request.Dl = URI.urlfile;
	}

	return URI;
}

static inline void prepare_cookies() {
	std::string cookiehdr = "";
    unsigned ecount = 0;
    for (const auto& elem : wrap::toSource.cookies_tosend)
	{	//note: cookies and their values will be URL encoded by default
		ecount++;
		if (wrap::toSource.cookie_encode) {
			//std::cout << "cookie encoding" << std::endl;
			cookiehdr += url_encode(elem.first) + "=" + url_encode(elem.second);
		}
		else {
			//std::cout << "no cookie encoding" << std::endl;
			cookiehdr += elem.first + "=" + elem.second;
		}

		if (ecount != wrap::toSource.cookies_tosend.size()) { // if its not the last one, separate with '; '
			cookiehdr += "; ";
		}
	}
	wrap::toSource.Header.hdr.insert(std::pair <std::string, std::string>("Cookie", cookiehdr));
	wrap::toSource.cookies_tosend.clear();
}

static inline std::string prepare_headers(wrap::req& request, const UriInfo& URI) {
	//this has to be here so that its processed after any change in cookie encoding settings
	prepare_cookies();
	// POST stuff done before headers to potentially add content-type if needed, before header assembly
	if (request.Method == "POST") {
		//if no content-type header was set, set it to application/x-www-form-urlencoded
		//rAnDOm capitalization will break this check
		if ((request.Header.hdr.find("content-type") == request.Header.hdr.end()) && (request.Header.hdr.find("Content-Type") == request.Header.hdr.end())) {
			request.Header.hdr.insert(std::pair<std::string, std::string>("Content-Type", "application/x-www-form-urlencoded"));
		}
	}

	//if its a GET request,  add content-type if needed, before header assembly
	if (request.Method == "GET") {
		//if no content-type header was set, set it to text/plain - n.b: rAnDOm capitalization will break this check
		if ((request.Header.hdr.find("content-type") == request.Header.hdr.end()) && (request.Header.hdr.find("Content-Type") == request.Header.hdr.end())) {
			request.Header.hdr.insert(std::pair<std::string, std::string>("Content-Type", "text/plain"));
		}
	}

	//assemble headers from map - note headers will not work with non ASCII chars  https://docs.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-httpsendrequestw
	std::string final_headers = "";
    for (const auto& elem : request.Header.hdr)
	{
		if (elem.first != "") {
			final_headers += elem.first + ":" + elem.second + "\r\n";
		}
		//this is done because WinINet automatically strips the Cookie header
		//and just goes by the cookies it can see that were set via WinINet (as below)
		if (elem.first == "Cookie") {
            std::string cookie_url = URI.scheme + "://" + URI.host;//ignoring path for now
			InternetSetCookieA(&cookie_url[0], NULL, &elem.second[0]); //set cookie via WinINet
		}
	}
	if (final_headers != "") {
		final_headers += "\r\n\r\n"; //null terminated string
	}
	return final_headers;
}

static inline std::string get_response(HINTERNET& hRequest, wrap::Response& output, wrap::req& request) {
	std::string strResponse;
	const int nBuffSize = 1024;
	char buff[nBuffSize]; //use bytes rather than string to help with binary file downloads

	FILE* pfile = nullptr;
	if (request.Dl != "") {
		pfile = fopen(request.Dl.c_str(), "wb");
	}

	BOOL bKeepReading = true;
	DWORD dwBytesRead = -1;

	while (bKeepReading && dwBytesRead != 0)
	{

		bKeepReading = InternetReadFile(hRequest, buff, nBuffSize, &dwBytesRead);
		if (dwBytesRead != 0) {
			output.downloaded_bytes += dwBytesRead;
		}

		if (pfile != nullptr) {
			fwrite(buff, sizeof(char), dwBytesRead, pfile);
		}
		else {
			strResponse.append(buff, dwBytesRead);
		}
	}
	if (pfile != nullptr) {
		fflush(pfile);
		fclose(pfile);
	}
	return strResponse;
}

static inline std::string get_statuscode_redirect(HINTERNET& hRequest, wrap::Response& output) {
	DWORD statusCode = 0;
	DWORD length = sizeof(DWORD);
	std::string redirect = "";

	if (HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &length, NULL)) {
		if ((statusCode > 299 && statusCode < 400) || (statusCode == 201)) { //treat code 3xx or 201 as a redirect
			//std::cout << "redirect" << std::endl;
			redirect = "on";
			output.redirect_count += 1;
		}
		else {//dont store the code unless theres no redirect or if there is, only last response
			output.status_code = std::to_string(statusCode);
		}
	}
	else {
		//error handling
	}
	return redirect;
}

static inline void get_recd_headers(HINTERNET& hRequest,wrap::req& request, wrap::Response & output,
                std::string& redirect, UriInfo& URI)
{
	//get headers recd
	std::string received_headers;
	//get the size headers
	DWORD d = 0;

	get_received_headers:

	// This call will fail on the first pass, because no buffer is allocated.
	if (!HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, &received_headers[0], &d, NULL))
	{
		if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
		{
			// Code to handle the case where the header isn't available.
		}
		else
		{
			// Check for an insufficient buffer.
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				received_headers.resize(d, '\0'); // Allocate the necessary buffer.
				goto get_received_headers;// Retry the call.
			}
			else
			{
				// Error handling code.
			}
		}
	}
	else { //no errors 
		if (received_headers != "") {
			//break headers into map - unless it was a redirect, in which case only set-cookies and location are actioned
			std::string delimiter = "\n";
			size_t pos = 0;
			std::string token, fieldname, value;
			while ((pos = received_headers.find(delimiter)) != std::string::npos) {
				token = received_headers.substr(0, pos);
				if (token.find(":") != std::string::npos) { //filters out lines without :, e.g. 200 OK
					//here we convert the header field name to lowercase
					//NOTE: this method does NOT support UTF-8 (only ascii) 
					//but should not be a problem as header field names are all standard ascii
					//header values are left as recd
					fieldname = token.substr(0, token.find(":"));
					for (auto& c : fieldname)
					{
						c = std::tolower(c);
					}
					value = token.substr(token.find(":") + 1);
					ltrim(value);
					if (redirect == "") {//only store headers if its not a redirect
						output.header.insert(std::pair<std::string, std::string>(fieldname, value));
					}

					//cookies are dealt with on the spot
					if (fieldname == "set-cookie") {
						std::string cname = value.substr(0, value.find("=")); //if theres more data than name=value, include it for now
						//std::cout <<"cookie set: "<< cname << std::endl;
                        std::string cookie_url = URI.scheme + "://";
						//if domain or path fields were set in cookie then use them, otherwise construct url from current host url
						size_t dm, DM, pth, PTH;
						dm = value.find("domain=") + 7; DM = value.find("Domain=") + 7; pth = value.find("path=") + 5; PTH = value.find("Path=") + 5;
						if ((dm != std::string::npos) || (DM != std::string::npos) || (pth != std::string::npos) || (PTH != std::string::npos)) {
							if (dm != std::string::npos) {
								cookie_url += value.substr(dm);
							}
							else if (DM != std::string::npos) {
								cookie_url += value.substr(DM);
							}
							if (cookie_url.find(";") != std::string::npos) {//if there was additional value data after then clip it off
								cookie_url = cookie_url.substr(0, cookie_url.find(";"));
							}
							if (pth != std::string::npos) {
								cookie_url += value.substr(pth);
							}
							else if (PTH != std::string::npos) {
								cookie_url += value.substr(PTH);
							}
                            if (auto pSemi = cookie_url.find(';'); pSemi != std::string::npos) {//if there was additional value data after then clip it off
                                cookie_url = cookie_url.substr(0, pSemi);
							}
						}
						else {
                            cookie_url += URI.host;//ignoring path for now
						}

						InternetSetCookieA(&cookie_url[0], &cname[0], &value[0]); //set cookie via WinINet
						//also save cookie value (only) into 'cookies' map obj in response
						//this assumes value is not used again in this iteration 
						value = value.substr(value.find("=") + 1);
						if (value.find(";") != std::string::npos) {//remove anything after the value
							value = value.substr(0, value.find(";"));
						}
						output.cookies.insert(std::pair<std::string, std::string>(cname, value));
					}

					if (fieldname == "location") {

						//figure out if its an absolute or relative URI
						//std::cout << "found location header" << std::endl << value << std::endl;
						if ((value.find("://") == std::string::npos)) {
							//if theres no scheme :// at the start assume this is a relative URI
							if (value[0] == '/') {
								//std::cout << "changing path" << std::endl ;
                                URI.path = value;
							}
							redirect = "relative";
						}
						else { // if it seems to be an absolute URI
							request.Url = value;
							redirect = "absolute";
						}
					}

					if (fieldname == "retry-after") { //in a redirection the server uses this to tell us how long to wait before next request
						if (redirect != "") {

						}
						else { //must be a 503 or 429 https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Retry-After

						}
					}

				}
				received_headers.erase(0, pos + delimiter.length());
			}
		}
	}

}

static inline void get_sent_headers(HINTERNET hRequest, wrap::Response& output) {
	std::string sent_headers;
	DWORD d = 0;

get_sent_headers:

	// This call will fail on the first pass, because no buffer is allocated.
	if (!HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF | HTTP_QUERY_FLAG_REQUEST_HEADERS, &sent_headers[0], &d, NULL))
	{
		if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
		{
			// Code to handle the case where the header isn't available.
		}
		else
		{
			// Check for an insufficient buffer.
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				sent_headers.resize(d, '\0'); // Allocate the necessary buffer.
				goto get_sent_headers;// Retry the call.
			}
			else
			{
				// Error handling code.
			}
		}
	}
	else { //no errors
		if (sent_headers != "") {
			//std::cout << std::endl << sent_headers << std::endl;
			//std::string s(sent_headers, d);
			//break headers std::string into map
			std::string delimiter = "\n";
			size_t pos = 0;
			std::string token;
			while ((pos = sent_headers.find(delimiter)) != std::string::npos) {

				token = sent_headers.substr(0, pos);
				//cout << "processing:\n" + token << endl;
				if (token.find(":") != std::string::npos) {
					std::string first = token.substr(0, token.find(":"));
					std::string second = token.substr(token.find(":") + 1);
					//cout << "adding: " + first +" " + second << endl;
					output.sent_headers.insert(std::pair<std::string, std::string>(token.substr(0, token.find(":")), token.substr(token.find(":") + 1)));
				}
				sent_headers.erase(0, pos + delimiter.length());
			}
		}
	}

}

#ifdef __MINGW32__
struct INTERNET_SECURITY_CONNECTION_INFO {
    uint32_t dwSize;
    BOOL fSecure;
    SecPkgContext_ConnectionInfo connectionInfo;
    SecPkgContext_CipherInfo cipherInfo;
};

constexpr unsigned INTERNET_OPTION_SECURITY_CONNECTION_INFO = 66;
#endif

template <class T>
inline void zmem(T& x) {
    static_assert(!std::is_pointer_v<T>, "Pointers forbidden for reliability");
    memset(&x, 0, sizeof(T));
}

static inline void get_secinfo(HINTERNET hRequest, wrap::Response& output) {
	//get security info - see here : https://stackoverflow.com/questions/41187935/can-not-programmatically-determine-which-tls-version-my-app-uses
    INTERNET_SECURITY_CONNECTION_INFO connInfo;
    zmem(connInfo);
	DWORD certInfoLength = sizeof(INTERNET_SECURITY_CONNECTION_INFO);
	InternetQueryOption(hRequest, INTERNET_OPTION_SECURITY_CONNECTION_INFO, &connInfo, &certInfoLength);
	//cout << connInfo.connectionInfo.dwProtocol << endl;
	switch (connInfo.connectionInfo.dwProtocol) {
	case(SP_PROT_TLS1_2_CLIENT): output.secinfo["protocol"] = "Transport Layer Security 1.2 client-side"; break;
	case(SP_PROT_TLS1_1_CLIENT): output.secinfo["protocol"] = "Transport Layer Security 1.1 client-side"; break;
	case(SP_PROT_TLS1_CLIENT): output.secinfo["protocol"] = "Transport Layer Security 1.0 client-side"; break;
	case(SP_PROT_TLS1_SERVER): output.secinfo["protocol"] = "Transport Layer Security 1.0 server-side"; break;
	case(SP_PROT_SSL3_CLIENT): output.secinfo["protocol"] = "Secure Sockets Layer 3.0 client-side."; break;
	case(SP_PROT_SSL3_SERVER): output.secinfo["protocol"] = "Secure Sockets Layer 3.0 server-side."; break;
	case(SP_PROT_TLS1_1_SERVER): output.secinfo["protocol"] = "Transport Layer Security 1.1 server-side."; break;
	case(SP_PROT_TLS1_2_SERVER): output.secinfo["protocol"] = "Transport Layer Security 1.2 server-side."; break;
	case(SP_PROT_SSL2_CLIENT): output.secinfo["protocol"] = "Secure Sockets Layer 2.0 client-side. Superseded by SP_PROT_TLS1_CLIENT."; break;
	case(SP_PROT_SSL2_SERVER): output.secinfo["protocol"] = "Secure Sockets Layer 2.0 server-side. Superseded by SP_PROT_TLS1_SERVER. "; break;
	case(SP_PROT_PCT1_CLIENT): output.secinfo["protocol"] = "Private Communications Technology 1.0 client-side. Obsolete."; break;
	case(SP_PROT_PCT1_SERVER): output.secinfo["protocol"] = "Private Communications Technology 1.0 server-side. Obsolete."; break;
	}

	switch (connInfo.connectionInfo.aiCipher) {
	case(CALG_3DES): output.secinfo["cipher"] = "3DES block encryption algorithm"; break;
	case(CALG_AES_128): output.secinfo["cipher"] = "AES 128-bit encryption algorithm"; break;
	case(CALG_AES_256): output.secinfo["cipher"] = "AES 256-bit encryption algorithm"; break;
	case(CALG_DES): output.secinfo["cipher"] = "DES encryption algorithm"; break;
	case(CALG_RC2): output.secinfo["cipher"] = "RC2 block encryption algorithm"; break;
	case(CALG_RC4): output.secinfo["cipher"] = "RC4 stream encryption algorithm"; break;
	case(0): output.secinfo["cipher"] = "No encryption"; break;
	}

	output.secinfo["cipher_strength"] = std::to_string(connInfo.connectionInfo.dwCipherStrength);

	switch (connInfo.connectionInfo.aiHash) {
	case(CALG_MD5): output.secinfo["hash"] = "MD5 hashing algorithm"; break;
	case(CALG_SHA): output.secinfo["hash"] = "SHA hashing algorithm"; break;
	}

	if (output.secinfo["hash"] != "") {
		output.secinfo["hash_strength"] = std::to_string(connInfo.connectionInfo.dwHashStrength);
	}

	switch (connInfo.connectionInfo.aiExch) {
	case(CALG_RSA_KEYX): output.secinfo["key_exch"] = "RSA key exchange"; break;
	case(CALG_DH_EPHEM): output.secinfo["key_exch"] = "Diffie-Hellman key exchange"; break;
	}

	if (output.secinfo["key_exch"] != "") {
		output.secinfo["key_exch_strength"] = std::to_string(connInfo.connectionInfo.dwExchStrength);
	}

	std::string cert_info_string;
	DWORD cert_info_length = 2048;
	cert_info_string.resize(cert_info_length, '\0');

	if (!InternetQueryOptionA(hRequest, INTERNET_OPTION_SECURITY_CERTIFICATE, &cert_info_string[0], &cert_info_length))
	{
		//output.err = "InternetQueryOption failed " + GetLastError();
		//return output;
	}
	else {
		output.secinfo["certificate"] = cert_info_string;
	}

}

static inline bool TimeoutInternetRequest(HINTERNET& hRequest, std::string final_headers, wrap::Response& output) {
	TRPARAMS rparams;
	rparams.hRequest = hRequest;
	rparams.headers = final_headers;
	HANDLE rhThread;
	rhThread = CreateThread(
		NULL,            // Pointer to thread security attributes 
		0,               // Initial thread stack size, in bytes 
		WorkerInternetRequest,  // Pointer to thread function 
		&rparams,     // lpParameter A pointer to a variable to be passed to the thread.
		0,               // Creation flags 
		0      // lpThreadId A pointer to a DWORD variable that receives the thread identifier.If this parameter is NULL, the thread identifier is not returned.
	);

	if (rhThread == 0) { //this is done to avoid possibility of WaitForSingleObject or GetExitCodeThread called if hThread is 0
		output.err = "Could not create thread for timeout.";
		return false;
	}

	// Wait for the call to InternetConnect in worker function to complete
	if (WaitForSingleObject(rhThread, wrap::toSource.TimeoutRequest) == WAIT_TIMEOUT)
	{
		std::cout << "Can not send request to server in " << wrap::toSource.TimeoutRequest << " milliseconds" << std::endl;
		if (wrap::toSource.session_handle)
			InternetCloseHandle(wrap::toSource.session_handle);
		// Wait until the worker thread exits
		WaitForSingleObject(rhThread, INFINITE);
		//std::cout << "Thread has exited" << std::endl;
		wrap::toSource.reset(); //reset to remove timeout for next request
		output.err = "InternetConnect Thread has exited ";
		return false;
	}

	// The state of the specified object (thread) is signaled
	DWORD dwExitCode = 0;
	if (!GetExitCodeThread(rhThread, &dwExitCode))
	{
		//std::cerr << "Error on GetExitCodeThread: " << GetLastError() << std::endl;
        output.err = "Error on GetExitCodeThread: " + std::to_string(GetLastError());
		return false;
	}

	CloseHandle(rhThread);

	if (dwExitCode) {
		// Worker function failed
		//std::cout << "non 0 exit code" << std::endl;
		output.err = "Worker function failed";
		return false;
	}

	return rparams.res;
}

static inline bool TimeoutInternetConnect(HINTERNET& hInternet, HINTERNET& hConnect, wrap::Response& output,std::string host,INTERNET_PORT port, DWORD service) {
	TPARAMS params;
	params.hInternet = hInternet;
	params.host = host;
	params.port = port;
	params.service = service;
	HANDLE hThread;
	hThread = CreateThread(
		NULL,            // Pointer to thread security attributes 
		0,               // Initial thread stack size, in bytes 
		WorkerInternetConnect,  // Pointer to thread function 
		&params,     // lpParameter A pointer to a variable to be passed to the thread.
		0,               // Creation flags 
		0      // lpThreadId A pointer to a DWORD variable that receives the thread identifier.If this parameter is NULL, the thread identifier is not returned.
	);

	if (hThread == 0) { //this is done to avoid possibility of WaitForSingleObject or GetExitCodeThread called if hThread is 0
		output.err = "Could not create thread for timeout.";
		return false;
	}

	// Wait for the call to InternetConnect in worker function to complete
	if (WaitForSingleObject(hThread, wrap::toSource.TimeoutConnect) == WAIT_TIMEOUT)
	{
		std::cout << "Can not connect to server in " << wrap::toSource.TimeoutConnect << " milliseconds" << std::endl;
		if (hInternet)
			InternetCloseHandle(hInternet);
		// Wait until the worker thread exits
		WaitForSingleObject(hThread, INFINITE);
		//std::cout << "Thread has exited" << std::endl;
		wrap::toSource.reset(); //reset timeout for next request
		output.err = "InternetConnect Thread has exited ";
		return false;
	}

	// The state of the specified object (thread) is signaled
	DWORD dwExitCode = 0;
	if (!GetExitCodeThread(hThread, &dwExitCode))
	{
		//std::cerr << "Error on GetExitCodeThread: " << GetLastError() << std::endl;
        output.err = "Error on GetExitCodeThread: " + std::to_string(GetLastError());
		return false;
	}

	CloseHandle(hThread);

	if (dwExitCode) {
		// Worker function failed
		//std::cout << "non 0 exit code" << std::endl;
		output.err = "Worker function failed";
		return false;
	}

	hConnect = params.hInternetOut;
	return true;
}

/////////////////// WorkerFunctions ////////////////////// 
DWORD WINAPI WorkerInternetConnect(IN LPVOID vThreadParm) // https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms686736(v=vs.85)
{
	TPARAMS* params;
	params = (TPARAMS*)vThreadParm;
	HINTERNET g_hConnect = 0;
    if (!(g_hConnect = InternetConnectA(params->hInternet, params->host.c_str(), params->port, NULL, NULL, params->service, 0, 0u)))
	{
		//std::cerr << "Error on InternetConnnect: " << GetLastError() << std::endl;
		return 1; // failure
	}
	else {
		//std::cout << "Connected OK" << std::endl;
		params->hInternetOut = g_hConnect;
	}
	return 0;  // success
}

DWORD WINAPI WorkerInternetRequest(IN LPVOID vThreadParm) // https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms686736(v=vs.85)
{
	TRPARAMS* params;
	params = (TRPARAMS*)vThreadParm;
	BOOL g_hConnect = false; 
	if (!(g_hConnect = HttpSendRequestA(params->hRequest, params->headers.c_str(), -1L, &wrap::toSource.PostData[0], sizeof(char) * wrap::toSource.PostData.size())))
	{
		//std::cerr << "Error on HttpSendRequestA: " << GetLastError() << std::endl;
		return 1; // failure
	}
	else {
		//std::cout << "Connected OK" << std::endl;
		params->res = g_hConnect;
	}
	return 0;  // success
}

// not a proper parser
std::string wrap::text_from_html(std::string html) {
	std::string output = "";
	bool over_tag = false; //whether we are currently going over a tag
	bool js = false;
	bool css = false;
	for (std::string::size_type i = 0; i < html.size(); ++i) {
		if (html[i] == '<') {
			if (html[i + 1] == '/') {
				//cout << "close tag" << endl;
				if (html.substr(i + 2, 5) == "style") {
					//cout << "close style tag" << endl;
					css = false;
				}
				if (html.substr(i + 2, 6) == "script") {
					//cout << "close js tag" << endl;
					js = false;
				}
			}
			else {
				//cout << "open tag" << endl;
				if (html.substr(i + 1, 5) == "style") {
					//cout << "open style tag" << endl;
					css = true;
				}
				if (html.substr(i + 1, 6) == "script") {
					//cout << "open js tag" << endl;
					js = true;
				}
			}
			over_tag = true;
		}
		if ((!js) && (!css) && (!over_tag)) {
			output = output + (html[i]);
		}
		if (html[i] == '>') {
			over_tag = false;
		}

	}

	return output;
}



//string trimming functions, from http://stackoverflow.com/questions/216823/ddg#217605

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
// static inline void rtrim(std::string& s) {
// 	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
// 		return !std::isspace(ch);
// 		}).base(), s.end());
// }

// // trim from both ends (in place)
// static inline void trim(std::string& s) {
// 	ltrim(s);
// 	rtrim(s);
// }




//adapted from https://stackoverflow.com/questions/20634666/get-a-mime-type-from-a-extension-in-c
std::string GetMimeType(const std::string& ext) {

    auto str = mojibake::toS<std::wstring>(ext);

	LPWSTR pwzMimeOut = NULL;
	HRESULT hr = FindMimeFromData(NULL, str.c_str(), NULL, 0,
		NULL, FMFD_URLASFILENAME, &pwzMimeOut, 0x0);
	if (SUCCEEDED(hr)) {
		std::wstring strResult(pwzMimeOut);
		// Despite the documentation stating to call operator delete, the
		// returned string must be cleaned up using CoTaskMemFree
		CoTaskMemFree(pwzMimeOut);
        auto narrow = mojibake::toS<std::string>(strResult);
		return narrow;
	}
	return "application/unknown";
}

static std::string base64_encode(const std::string& in) {

	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

std::string hex_encode(char const c)
{
	char s[3];

	if (c & 0x80)
	{
		std::snprintf(&s[0], 3, "%02X",
			static_cast<unsigned int>(c & 0xff)
		);
	}
	else
	{
		std::snprintf(&s[0], 3, "%02X",
			static_cast<unsigned int>(c)
		);
	}

	return std::string(s);
}

std::string url_encode(std::string const& str)
{
	std::string res;
	res.reserve(str.size());
	bool form = true;
	for (auto const& e : str)
	{
		if (e == ' ' && form)
		{
			res += "+";
		}
		else if (std::isalnum(static_cast<unsigned char>(e)) ||
			e == '-' || e == '_' || e == '.' || e == '~')
		{
			res += e;
		}
		else
		{
			res += "%" + hex_encode(e);
		}
	}

	return res;
}


////////////////////////////////// PARAMETERS

void wrap::Params(wrap::Parameters p) {

	//assemble parameters from map
	std::string final_params = "";
	int pcount = 0;
    for (const auto& elem : p.p)
	{
		pcount++;
		if (elem.first != "") {
			if (pcount > 1) { // add & if we're after 1st param
				final_params += "&";
			}
			final_params += url_encode(elem.first) + "=" + url_encode(elem.second);
		}
	}
	wrap::toSource.Params = "?" + final_params;
	//std::cout << "Parameters url encoded:" << std::endl;
	//std::cout << wrap::toSource.Params << std::endl;
}

void wrap::Params(wrap::Url u) {
	wrap::toSource.Url = u.adr;
	//std::cout << "got url: " + u.adr << std::endl;
}

void wrap::Params(wrap::Proxy p) {
	wrap::toSource.Proxy = p;
	wrap::toSource.AccessType = INTERNET_OPEN_TYPE_PROXY;
	wrap::toSource.newsession = true;
}

void wrap::Params(wrap::UserAgent usr) {
	wrap::toSource.ua = usr.usr;
	wrap::toSource.newsession = true;
}

void wrap::Params(wrap::Options o) {
    for (const auto& elem : o.options) {
		if ((elem.first.find("encod") != std::string::npos) && (elem.first.find("cookie") != std::string::npos)) {
			if ((elem.second.find("off") != std::string::npos) || (elem.second.find("false") != std::string::npos)) {
				wrap::toSource.cookie_encode = false;
			}
			if ((elem.second.find("on") != std::string::npos) || (elem.second.find("true") != std::string::npos)) {
				wrap::toSource.cookie_encode = true;
			}
		}
		if (elem.first.find("redirect") != std::string::npos)  {
			if ((elem.second.find("off") != std::string::npos) || (elem.second.find("false") != std::string::npos)) {
				wrap::toSource.redirect = false;
			}
			if ((elem.second.find("on") != std::string::npos) || (elem.second.find("true") != std::string::npos)) {
				wrap::toSource.redirect = true;
			}
		}
	}
}

void wrap::Params(wrap::Header h) {
	wrap::toSource.Header = h;
}

void wrap::Params(wrap::Cookies c) { //https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
//just add cookies as header item - even though its the last item it should be ok (order doesnt matter)
	wrap::toSource.cookies_tosend = c.c;
}

void wrap::Params(wrap::Method m) {
	wrap::toSource.Method = m.method;
}

void wrap::Params(wrap::Download dl) {
	wrap::toSource.Dl = dl.dl;
}

void wrap::Params(wrap::File f) {
	std::string fname = f.file;
	// read entire file into string
	if (std::ifstream inputstream{ fname, std::ios::binary | std::ios::ate }) {
		auto fsize = inputstream.tellg();
		std::string finput(fsize, '\0'); // construct string to stream size
		inputstream.seekg(0);
		if (inputstream.read(&finput[0], fsize))
			std::cout << finput << '\n';
		wrap::toSource.PostData = finput;
	}
}

void wrap::Params(wrap::Body b) {
	wrap::toSource.PostData = b.body;
}

void wrap::Params(wrap::Payload pd) {
	//assemble payload from map
	std::string final_postdata = "";
	int pcount = 0;
    for (const auto& elem : pd.pd)
	{
		pcount++;
		if (elem.first != "") {
			if (pcount > 1) { // add & if we're after 1st param
				final_postdata += "&";
			}
			final_postdata += url_encode(elem.first) + "=" + url_encode(elem.second);
		}
	}
	wrap::toSource.PostData = final_postdata;
	//std::cout << "Payload data url encoded:" << std::endl;
	//std::cout << toSource.PostData << std::endl;
}

void wrap::Params(wrap::Multipart mp) {
	//assemble multipart post payload from map
	std::string final_postdata = "";
	std::string boundary = "735323031399963166993862150";
	wrap::toSource.Header.hdr["content-type"] = "multipart/form-data; boundary=" + boundary;
	std::string fname, ctype;
    unsigned pcount = 0;
    for (const auto& elem : mp.mp)
	{
		pcount++;

		if (elem.first.substr(0, 5) != "file:") { // if theres no file: prefix, assume this is not a file

			final_postdata += "--" + boundary + "\r\n";
			final_postdata += "Content-Disposition: form-data; name=\"" + (elem.first) + "\"\r\n";
			final_postdata += "Content-Type: text/plain\r\n\r\n";
			final_postdata += (elem.second) + "\r\n";
		}
		else { //if we're sending a file

			fname = elem.second.substr(elem.second.find_last_of("/\\") + 1); //used for filename
			ctype = "application/octet-stream"; //used as default for file content-type

			//try to see if we can match the MIME type, by getting the file extension
			if (fname.find_last_of(".") != std::string::npos) { //use find_last_of to ignore dots in filename 
				std::string extension = fname.substr(fname.find_last_of("."));
				if (GetMimeType(extension) != "application/unknown") {
					ctype = GetMimeType(extension);
				}
			}

			final_postdata += "--" + boundary + "\r\n";
			final_postdata += "Content-Disposition: form-data; name=\"" + (elem.first.substr(5)) + "\"; filename=\"" + fname + "\"\r\n";
			final_postdata += "Content-Type: " + ctype + "\r\n\r\n";

			// read entire file into string
			if (std::ifstream inputstream{ elem.second, std::ios::binary | std::ios::ate }) {
				auto fsize = inputstream.tellg();
				std::string finput(fsize, '\0'); // construct string to stream size
				inputstream.seekg(0);
				inputstream.read(&finput[0], fsize);
				final_postdata += finput + "\r\n"; //add to postdata
			}
		}

		if (pcount == mp.mp.size()) { //if we've reached the last map element, add -- to the end of the last boundary, as per RFC spec
			final_postdata += "--" + boundary + "--";
		}
	}
	wrap::toSource.PostData = final_postdata;
	//std::cout << "multipart payload:" << std::endl;
	//std::cout << final_postdata << std::endl;
}

void wrap::Params(wrap::Authentication auth) {
	std::string basicauth = auth.usr + ":" + auth.pwd;
	basicauth = base64_encode(basicauth);
	wrap::toSource.Header.hdr["Authorization"] = "Basic " + basicauth;
}

void wrap::Params(wrap::Bearer token) { //https://www.oauth.com/oauth2-servers/making-authenticated-requests/
	wrap::toSource.Header.hdr["Authorization"] = "Bearer " + token.token;
}

void wrap::Params(wrap::Timeout timeout) {

	if (timeout.type == "connection" || timeout.type == "all") {
		wrap::toSource.TimeoutConnect = timeout.timeout;
	}
	if (timeout.type == "request" || timeout.type == "all") {
		wrap::toSource.TimeoutRequest = timeout.timeout;
	}
}
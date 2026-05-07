#include "MyHttpClient.h"

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXHttpClient.h>
#include <ixwebsocket/IXSocketTLSOptions.h>

///// MyHttpThread /////////////////////////////////////////////////////////////

std::mutex initMutex;
std::atomic<bool> isIxHere = false;

void initIx()
{
    if (!isIxHere) {
        std::lock_guard _(initMutex);
        if (!isIxHere) {
            ix::initNetSystem();
            isIxHere = true;
        }
    }
}


void detail::MyHttpThread::run()
{
    state = State::WORKING;
    tempResponse.clear();
    httpCode = 0;
    error = MyHttpError::OK;

    initIx();

    ix::HttpClient httpClient;

    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE"; //"cacert.pem";
    tlsOptions.tls = true;
    httpClient.setTLSOptions(tlsOptions);
    ix::WebSocketHttpHeaders headers;

    auto args = httpClient.createRequest(owner.url());

    args->extraHeaders = headers;
    args->followRedirects = true;
    args->maxRedirects = 10;

    auto response = httpClient.get(owner.url(), args);

    tempResponse = std::move(response->body);
    httpCode = response->statusCode;
    error = (response->errorCode == ix::HttpErrorCode::Ok)
                ? MyHttpError::OK : MyHttpError::MISC;
}


///// MyHttpClient /////////////////////////////////////////////////////////////

MyHttpClient::MyHttpClient() : thread(*this)
{
    connect(&thread, &detail::MyHttpThread::finished,
            this, &This::threadEnded);
}


int MyHttpClient::httpCode() const { return fHttpCode; }
const std::string& MyHttpClient::response() const { return fResponse; }
MyHttpError MyHttpClient::error() const { return fError; }

bool MyHttpClient::start(std::string_view url)
{
    if (fState == State::WORKING)
        return false;
    fUrl.assign(url);
    thread.wait();
    fState = State::WORKING;
    thread.start();
    return true;
}


MyHttpClient::~MyHttpClient()
{
    thread.state = detail::MyHttpThread::State::HALT;
    thread.wait();
}


void MyHttpClient::threadEnded()
{
    fResponse = std::move(thread.tempResponse);
    fError = thread.error;
    fHttpCode = thread.httpCode;
    fState = State::IDLE;

    emit requestEnded(*this);
}

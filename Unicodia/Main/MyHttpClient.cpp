#include "MyHttpClient.h"

#include "wnetwrap.h"

///// MyHttpThread /////////////////////////////////////////////////////////////

void detail::MyHttpThread::run()
{
    state = State::WORKING;
    tempResponse.clear();
    httpCode = 0;
    error = MyHttpError::OK;

    wrap::Response r = wrap::HttpsRequest(wrap::Url{owner.url()});
    tempResponse = std::move(r.text);
    httpCode = atoi(r.status_code.c_str());
    error = r.err.empty() ? MyHttpError::OK : MyHttpError::MISC;
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

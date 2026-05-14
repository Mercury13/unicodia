#include "MyHttpClient.h"

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXHttpClient.h>

///// detail::Thread ///////////////////////////////////////////////////////////

namespace {

    std::mutex initMutex;
    constinit std::atomic<bool> isIxHere = false;

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

    class InitIx {
    public:
        InitIx() { initIx(); }
        ~InitIx() = default;
    };

} // anon namespace


class myht::ClientImpl
{
public:
    [[nodiscard]] Result run(const std::string& url);
private:
    InitIx initIx;
    ix::HttpClient client;
};

myht::Result myht::ClientImpl::run(const std::string& url)
{
    ix::WebSocketHttpHeaders headers;

    std::string urlCopy(url);
    auto args = client.createRequest(url);
    args->extraHeaders = headers;
    args->followRedirects = true;
    args->maxRedirects = 10;

    auto response = client.get(url, args);

    return {
        .body = std::move(response->body),
        .errorMsg = std::move(response->errorMsg),
        .code = response->statusCode,
        .error = (response->errorCode == ix::HttpErrorCode::Ok)
                    ? Error::OK : Error::MISC,
    };
}

void myht::detail::Thread::run()
{
    state = State::WORKING;
    result = Result();

    myht::ClientImpl client;
    result = client.run(owner.url());
}


///// AsyncClient //////////////////////////////////////////////////////////////

myht::AsyncClient::AsyncClient() : thread(*this)
{
    connect(&thread, &detail::Thread::finished,
            this, &This::threadEnded);
}


bool myht::AsyncClient::start(std::string_view url)
{
    if (fState == State::WORKING)
        return false;
    fUrl.assign(url);
    thread.wait();
    fState = State::WORKING;
    thread.start();
    return true;
}


myht::AsyncClient::~AsyncClient()
{
    thread.state = detail::Thread::State::HALT;
    thread.wait();
}


void myht::AsyncClient::threadEnded()
{
    fResult = std::move(thread.result);
    fState = State::IDLE;

    emit requestEnded(fResult);
}


///// SyncClient ///////////////////////////////////////////////////////////////

myht::SyncClient::SyncClient() : impl(new ClientImpl) {}
myht::SyncClient::~SyncClient() = default;

myht::Result myht::SyncClient::run(const std::string& url)
{
    return impl->run(url);
}

#include "MyHttpClient.h"

#include "mongoose.h"

#include <iostream>


namespace mg {

    class Mgr : public mg_mgr {
    public:
        Mgr();
        ~Mgr();
    };

}

mg::Mgr::Mgr()  { mg_mgr_init(this); }
mg::Mgr::~Mgr() { mg_mgr_free(this); }


///// MyHttpThread /////////////////////////////////////////////////////////////

namespace {

    constinit const mg_str EMPTY_STR { .buf = nullptr, .len = 0 };

    static void ev_handler(struct mg_connection *c, int ev, void *p) {
        auto* that = reinterpret_cast<detail::MyHttpThread*>(c->fn_data);
        switch (ev) {
        case MG_EV_CONNECT: {
                // Connected to server. Extract host name from URL
                if (c->is_tls) {
                    //auto host = mg_url_host(that->owner.url().c_str());
                    mg_tls_opts opts {
                            .ca = EMPTY_STR, .cert = EMPTY_STR,
                            .key = EMPTY_STR, .name = EMPTY_STR, //host,
                            .skip_verification = 1 };
                    mg_tls_init(c, &opts);
                }
            } break;
        case MG_EV_HTTP_HDRS: {
                std::cout << "Headers!" << std::endl;
            } break;
        case MG_EV_HTTP_MSG: {
                auto* hm = reinterpret_cast<mg_http_message*>(p);
                that->tempResponse.append(hm->message.buf, hm->message.len);
            } break;
        case MG_EV_ERROR: {
                auto* msg = reinterpret_cast<char*>(p);
                std::cout << "Error: " << msg << std::endl;
                that->error = MyHttpError::MISC;
                that->state = detail::MyHttpThread::State::HALT;
            } break;
        case MG_EV_CLOSE: {
                that->state = detail::MyHttpThread::State::HALT;
            } break;
        default:;
        }
    }

}   // anon mamespace

void detail::MyHttpThread::run()
{
    state = State::WORKING;
    tempResponse.clear();
    httpCode = 0;
    error = MyHttpError::OK;

    mg::Mgr mgr;
    mg_http_connect(&mgr, owner.url().c_str(), ev_handler, this);

    while (state == State::WORKING) {
        mg_mgr_poll(&mgr, 1000);
    }
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

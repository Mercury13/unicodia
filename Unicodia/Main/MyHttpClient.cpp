#include "MyHttpClient.h"

#include <iostream>

// Poco
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/URI.h>

using namespace Poco::Net;
using namespace Poco;

///// MyHttpThread /////////////////////////////////////////////////////////////

void detail::MyHttpThread::run()
{
    state = State::WORKING;
    tempResponse.clear();
    httpCode = 0;
    error = MyHttpError::OK;

    // 1. Инициализация SSL-подсистемы
    // Используем AcceptCertificateHandler, чтобы не мучиться с сертификатами (для теста)
    SharedPtr<InvalidCertificateHandler> pHandler = new AcceptCertificateHandler(false);

    // Создаем контекст. TLS 1.2+ обычно включен по умолчанию в современных версиях
    Context::Ptr pContext = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

    SSLManager::instance().initializeClient(nullptr, pHandler, pContext);

    try {
        URI uri(owner.url());

        // 2. Создаем сессию
        HTTPSClientSession session(uri.getHost(), uri.getPort(), pContext);

        // 3. Формируем запрос
        HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
        request.set("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/147.0.0.0 Safari/537.36");
        session.sendRequest(request);

        // 4. Получаем ответ
        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);

        std::cout << "Status: " << response.getStatus() << " " << response.getReason() << std::endl;
        this->httpCode = response.getStatus();

        // Читаем тело ответа
        Poco::StreamCopier::copyToString(rs, this->tempResponse);
        std::cout << "Body length: " << tempResponse.length() << std::endl;

    } catch (Poco::Exception& ex) {
        std::cout << "Poco Exception: " << ex.displayText() << std::endl;
        error = MyHttpError::MISC;
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

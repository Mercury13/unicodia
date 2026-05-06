#pragma once

// C++
#include <string>
#include <atomic>

// Qt
#include <QObject>
#include <QThread>

class MyHttpClient;

constexpr int HT_OK = 200;
constexpr int HT_IM_A_TEAPOT = 418;

enum class MyHttpError : unsigned char {
    OK,
    MISC,  // misc errors
};

namespace detail {

    class MyHttpThread : public QThread
    {
    public:
        enum class State : unsigned char {
            WORKING, HALT };
        MyHttpClient& owner;
        State state = State::WORKING;
        std::string tempResponse;
        int httpCode = 0;
        MyHttpError error = MyHttpError::MISC;

        MyHttpThread(MyHttpClient& aOwner) : owner(aOwner) {}
    protected:
        void run() override;
    };

}


class MyHttpObject {  // interface
public:
    virtual int httpCode() const = 0;
    virtual const std::string& response() const = 0;
    virtual MyHttpError error() const = 0;

    bool isOk() const { return (error() == MyHttpError::OK); }
};


///
/// \brief The MyHttpClient class
///    Simple threaded HTTP(s) client
///
class MyHttpClient : public QObject, public MyHttpObject
{
    Q_OBJECT

    using This = MyHttpClient;
public:
    enum class State : unsigned char {
        IDLE, WORKING
    };

    MyHttpClient();
    MyHttpClient(const MyHttpClient&) = delete;
    ~MyHttpClient();

    State state() const;
    const std::string& url() const { return fUrl; }

    /// @return [+] started  [-] already working
    bool start(std::string_view url);

    // MyHttpObject
    int httpCode() const override;
    const std::string& response() const override;
    MyHttpError error() const override;

private:
    detail::MyHttpThread thread;
    std::atomic<State> fState = State::IDLE;
    std::atomic<int> fHttpCode { 0 };
    std::string fUrl;
    std::string fResponse;
    MyHttpError fError = MyHttpError::OK;

signals:
    void requestEnded(const MyHttpObject& obj);

private slots:
    void threadEnded();
};

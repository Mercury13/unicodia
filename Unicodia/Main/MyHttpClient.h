#pragma once

// C++
#include <string>
#include <atomic>

// Qt
#include <QObject>
#include <QThread>


class MyHttpClient;

namespace myht {

    constexpr int C_OK = 200;
    constexpr int C_IM_A_TEAPOT = 418;

    enum class Error : unsigned char {
        OK,
        MISC,  // misc errors
        DID_NOT_RUN,
    };

    struct Result {
        std::string body;
        std::string errorMsg;
        int code = 0;
        Error error = Error::DID_NOT_RUN;

        constexpr operator bool() const noexcept { return (error == Error::OK); }
    };

    class ClientImpl;
    class AsyncClient;

    namespace detail {

        class Thread : public QThread
        {
        public:
            enum class State : unsigned char {
                WORKING, HALT };
            AsyncClient& owner;
            State state = State::WORKING;
            Result result;

            Thread(AsyncClient& aOwner) : owner(aOwner) {}
        protected:
            void run() override;
        };

    }

    ///
    ///  Simple asynchronous HTTP(s) client
    ///
    class AsyncClient : public QObject
    {
        Q_OBJECT

        using This = AsyncClient;
    public:
        enum class State : unsigned char {
            IDLE, WORKING
        };

        AsyncClient();
        AsyncClient(const AsyncClient&) = delete;
        ~AsyncClient();

        State state() const;
        const std::string& url() const { return fUrl; }

        /// @return [+] started  [-] already working
        bool start(std::string_view url);
        const Result& result() { return fResult; }

    private:
        detail::Thread thread;
        std::atomic<State> fState = State::IDLE;
        std::string fUrl;
        Result fResult;
    signals:
        void requestEnded(const myht::Result& result);

    private slots:
        void threadEnded();
    };

    class SyncClient
    {
    public:
        SyncClient();
        ~SyncClient();
        [[nodiscard]] Result run(const std::string& url);
    private:
        std::unique_ptr<ClientImpl> impl;
    };
}
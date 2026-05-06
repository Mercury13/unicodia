#include <cstdio>

#include "mongoose.h"

static int s_done = 0; // Флаг завершения запроса

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_OPEN) {
        // Соединение открыто
    } else if (ev == MG_EV_CONNECT) {
        // Включаем TLS для этого соединения
        struct mg_tls_opts tls_opts = {0};
        tls_opts.ca.buf = NULL; // Не проверять сертификат (ТОЛЬКО ДЛЯ ТЕСТА!)
        tls_opts.ca.len = 0;    // Не проверять сертификат (ТОЛЬКО ДЛЯ ТЕСТА!)
        tls_opts.skip_verification = 1;
        mg_tls_init(c, &tls_opts);        // TLS подключение установлено (если адрес https)
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        printf("Got response:\n%.*s\n", (int) hm->body.len, hm->body.buf);
        c->is_closing = 1;         // Закрываем соединение
        s_done = 1;                // Выходим из цикла
    } else if (ev == MG_EV_ERROR) {
        printf("Err: %s\n", (char *) ev_data);
        s_done = 1;
    } else if (ev == MG_EV_CLOSE) {
        s_done = 1;
    }
}

int main() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);        // Инициализация

    // Для Windows 7/HTTPS обязательно:
    // 1. Компиляция с -DMG_ENABLE_OPENSSL=1 или -DMG_ENABLE_MBEDTLS=1
    // 2. Если не проверяешь сертификаты (для теста), можно не настраивать CA_bundle

    mg_http_connect(&mgr, "https://example.com", fn, NULL);

    while (!s_done) {
        mg_mgr_poll(&mgr, 100); // Крутим цикл, пока не получим ответ
    }

    mg_mgr_free(&mgr);
    return 0;
}
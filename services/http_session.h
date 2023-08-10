#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "http_fail.h"
#include "http_request.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

#ifndef SYSTEM_HTTP_SESSION_H
#define SYSTEM_HTTP_SESSION_H

class http_session : public std::enable_shared_from_this<http_session> {
    beast::ssl_stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_;
    std::shared_ptr<std::string const> doc_root_;
    http::request<http::string_body> req_;

public:
    explicit http_session(tcp::socket&& socket, ssl::context& ctx, std::shared_ptr<std::string const> const& doc_root) : stream_(std::move(socket), ctx) , doc_root_(doc_root) { }

    void run();
    void on_run();

    void on_handshake(beast::error_code ec);

    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void send_response(http::message_generator&& msg);

    void on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred);
    void do_close();
    void on_shutdown(beast::error_code ec);
};

#endif //SYSTEM_HTTP_SESSION_H

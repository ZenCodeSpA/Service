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
#include "http_session.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

#ifndef SYSTEM_HTTP_LISTENER_H
#define SYSTEM_HTTP_LISTENER_H

class http_listener : public std::enable_shared_from_this<http_listener> {
    net::io_context& ioc_;
    ssl::context& ctx_;
    tcp::acceptor acceptor_;
    std::shared_ptr<std::string const> doc_root_;

public:
    http_listener(net::io_context& ioc, ssl::context& ctx, tcp::endpoint endpoint, std::shared_ptr<std::string const> const& doc_root);

    void run();
private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};


#endif //SYSTEM_HTTP_LISTENER_H

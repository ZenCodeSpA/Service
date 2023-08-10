#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

#ifndef SYSTEM_HTTP_FAIL_H
#define SYSTEM_HTTP_FAIL_H

class http_fail {
public:
    static void make(beast::error_code ec, char const* what);
};

#endif //SYSTEM_HTTP_FAIL_H

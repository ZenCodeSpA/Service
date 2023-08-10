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

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

#ifndef SYSTEM_HTTP_REQUEST_H
#define SYSTEM_HTTP_REQUEST_H

class http_request {
public:
    static std::string path_cat(beast::string_view base, beast::string_view path);
    static beast::string_view mime_type(beast::string_view path);
};

#endif //SYSTEM_HTTP_REQUEST_H

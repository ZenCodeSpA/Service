#include "http_session.h"

template<class Body, class Allocator>
http::message_generator handle_request(beast::string_view doc_root, http::request<Body, http::basic_fields<Allocator>>&& req) {
    auto const bad_request =
            [&req](beast::string_view why)
            {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::string(why);
                res.prepare_payload();
                return res;
            };

    auto const not_found =
            [&req](beast::string_view target)
            {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + std::string(target) + "' was not found.";
                res.prepare_payload();
                return res;
            };

    auto const server_error =
            [&req](beast::string_view what)
            {
                http::response<http::string_body> res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + std::string(what) + "'";
                res.prepare_payload();
                return res;
            };

    if( req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return bad_request("Unknown HTTP-method");

    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    std::string path = http_request::path_cat(doc_root, req.target());
    if(req.target().back() == '/')
        path.append("index.html");

    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    if(ec == beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    if(ec)
        return server_error(ec.message());

    auto const size = body.size();

    if(req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, http_request::mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // Respond to GET request
    http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, http_request::mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

void http_session::run() {
    net::dispatch(
            stream_.get_executor(),
            beast::bind_front_handler(
                    &http_session::on_run,
                    shared_from_this()));
}

void http_session::on_run() {
    beast::get_lowest_layer(stream_).expires_after(
            std::chrono::seconds(30));

    stream_.async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                    &http_session::on_handshake,
                    shared_from_this()));
}

void http_session::on_handshake(beast::error_code ec)  {
    if(ec)
        return http_fail::make(ec, "handshake");

    do_read();
}

void http_session::do_read() {
    req_ = {};

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    http::async_read(stream_, buffer_, req_,
                     beast::bind_front_handler(
                             &http_session::on_read,
                             shared_from_this()));
}

void http_session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec == http::error::end_of_stream)
        return do_close();

    if(ec)
        return http_fail::make(ec, "read");

    send_response(
            handle_request(*doc_root_, std::move(req_)));
}

void http_session::send_response(http::message_generator &&msg) {
    bool keep_alive = msg.keep_alive();

    beast::async_write(
            stream_,
            std::move(msg),
            beast::bind_front_handler(
                    &http_session::on_write,
                    this->shared_from_this(),
                    keep_alive));
}

void http_session::on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return http_fail::make(ec, "write");

    if(! keep_alive) {
        return do_close();
    }

    do_read();
}

void http_session::do_close() {
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    stream_.async_shutdown(
            beast::bind_front_handler(
                    &http_session::on_shutdown,
                    shared_from_this()));
}

void http_session::on_shutdown(beast::error_code ec) {
    if(ec)
        return http_fail::make(ec, "shutdown");
}

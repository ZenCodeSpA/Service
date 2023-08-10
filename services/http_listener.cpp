#include "http_listener.h"

http_listener::http_listener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint,
                             const std::shared_ptr<const std::string> &doc_root): ioc_(ioc), ctx_(ctx), acceptor_(ioc), doc_root_(doc_root) {
    beast::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        http_fail::make(ec, "open");
        return;
    }

    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        http_fail::make(ec, "set_option");
        return;
    }

    acceptor_.bind(endpoint, ec);
    if (ec) {
        http_fail::make(ec, "bind");
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        http_fail::make(ec, "listen");
        return;
    }
}

void http_listener::run() {
    do_accept();
}

void http_listener::do_accept() {
    acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                    &http_listener::on_accept,
                    shared_from_this()));
}

void http_listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        http_fail::make(ec, "accept");
        return;
    } else {
        std::make_shared<http_session>(
                std::move(socket),
                ctx_,
                doc_root_)->run();
    }

    do_accept();
}

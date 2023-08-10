#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/program_options.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "services/http_listener.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
namespace po = boost::program_options;

inline void ssl_certificates(boost::asio::ssl::context& ctx) {
    ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::single_dh_use);
    ctx.use_certificate_chain_file("certificates/certificate.pem");
    ctx.use_private_key_file("certificates/private_key.pem", boost::asio::ssl::context::pem);
    ctx.use_tmp_dh_file("certificates/params.pem");
}

int main(int argc, char *argv[]) {
    int http_service_port;
    int http_threads_number;
    std::string http_directory_path;

    po::options_description desc("ZenCore System — Program Options");
    desc.add_options()
            ("help", "Print PO details")
            ("run",  "Run the Program")
            ("http_port", po::value<int>(&http_service_port), "The port number used by HTTP Service")
            ("http_threads", po::value<int>(&http_threads_number), "The number of threads used by HTTP Service")
            ("http_directory", po::value<std::string>(&http_directory_path), "The directory used by HTTP Service")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("run")) {
        tcp::endpoint endpoint;

        if (vm.count("http_port")) {
            endpoint = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(http_service_port) };
        } else {
            endpoint = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(8080) };
        }

        boost::asio::io_context ioc;

        ssl::context ctx { ssl::context::tlsv12 };
        ssl_certificates(ctx);

        std::shared_ptr<std::string> doc_root;

        if (vm.count("http_directory")) {
            doc_root = std::make_shared<std::string>(http_directory_path);
        } else {
            doc_root = std::make_shared<std::string>("www");
        }

        std::make_shared<http_listener>(ioc,ctx, endpoint, doc_root)->run();

        std::vector<std::thread> http_threads;
        if (vm.count("http_threads")) {
            http_threads.reserve(http_threads_number);
            for(auto i = http_threads_number; i > 0; --i)
                http_threads.emplace_back([&ioc]{ ioc.run(); });
        } else {
            http_threads.reserve(3);
            for(auto i = 3; i > 0; --i)
                http_threads.emplace_back([&ioc]{ ioc.run(); });
        }

        ioc.run();
    }

    return 0;
}

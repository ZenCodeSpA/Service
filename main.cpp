#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/program_options.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "services/http_listener.h"
#include "services/http_configuration.h"

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
    std::shared_ptr<http_configuration> http_configuration_ = std::make_shared<http_configuration>();

    po::options_description desc("ZenCore System — Program Options");

    desc.add_options()
            ("help", "Print PO details")
            ("run",  "Run the Program")
            ("http", po::value<bool>(&http_configuration_->enabled), "The HTTP Service module is enabled \n(boolean, on|off, default=on)")
            ("http_port", po::value<int>(&http_configuration_->port), "The port number used by HTTP Service \n(network port, default=8080)")
            ("http_read_timeout", po::value<long>(&http_configuration_->read_timeout), "The read timeout used by HTTP Service \n(seconds, default=30)")
            ("http_write_timeout", po::value<long>(&http_configuration_->write_timeout), "The write timeout used by HTTP Service \n(seconds, default=30)")
            ("http_handshake_timeout", po::value<long>(&http_configuration_->handshake_timeout), "The handshake timeout used by HTTP Service \n(seconds, default=30)")
            ("http_threads", po::value<int>(&http_configuration_->threads), "The number of threads used by HTTP Service \n(quantity, default=3)")
            ("http_directory", po::value<std::string>(&http_configuration_->directory), "The directory used by HTTP Service \n(path, default=www)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("run")) {

        if (vm.count("http")) {
            tcp::endpoint endpoint;

            if (vm.count("http_port")) {
                endpoint = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(http_configuration_->port) };
            } else {
                endpoint = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(8080) };
            }

            boost::asio::io_context ioc;

            ssl::context ctx { ssl::context::tlsv12 };
            ssl_certificates(ctx);

            std::shared_ptr<std::string> doc_root;

            if (vm.count("http_directory")) {
                doc_root = std::make_shared<std::string>(http_configuration_->directory);
            } else {
                doc_root = std::make_shared<std::string>("www");
            }

            if (!vm.count("http_read_timeout")) {
                http_configuration_->read_timeout = 30;
            }

            if (!vm.count("http_write_timeout")) {
                http_configuration_->write_timeout = 30;
            }

            if (!vm.count("http_handshake_timeout")) {
                http_configuration_->handshake_timeout = 30;
            }

            std::make_shared<http_listener>(ioc,ctx, endpoint, doc_root, http_configuration_)->run();

            std::vector<std::thread> http_threads;
            if (vm.count("http_threads")) {
                http_threads.reserve(http_configuration_->threads);
                for(auto i = http_configuration_->threads; i > 0; --i)
                    http_threads.emplace_back([&ioc]{ ioc.run(); });
            } else {
                http_threads.reserve(3);
                for(auto i = 3; i > 0; --i)
                    http_threads.emplace_back([&ioc]{ ioc.run(); });
            }

            ioc.run();
        }
    }

    return 0;
}

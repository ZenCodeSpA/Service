#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "services/http_listener.h"
#include "services/http_configuration.h"
#include "services/queue_configuration.h"
#include "services/queue_task.h"

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
    std::shared_ptr<queue_configuration> queue_configuration_ = std::make_shared<queue_configuration>();

    po::options_description options_description_("ZenCore System — Program Options");

    options_description_.add_options()
            ("help", "Print PO details")
            ("run",  "Run the Program")
            ("http", po::value<bool>(&http_configuration_->enabled), "The HTTP Service module is enabled \n(boolean, on|off, default=off)")
            ("http_port", po::value<int>(&http_configuration_->port), "The port number used by HTTP Service \n(network port, default=8080)")
            ("http_read_timeout", po::value<long>(&http_configuration_->read_timeout), "The read timeout used by HTTP Service \n(seconds, default=30)")
            ("http_write_timeout", po::value<long>(&http_configuration_->write_timeout), "The write timeout used by HTTP Service \n(seconds, default=30)")
            ("http_handshake_timeout", po::value<long>(&http_configuration_->handshake_timeout), "The handshake timeout used by HTTP Service \n(seconds, default=30)")
            ("http_threads", po::value<int>(&http_configuration_->threads), "The number of threads used by HTTP Service \n(quantity, default=3)")
            ("http_directory", po::value<std::string>(&http_configuration_->directory), "The directory used by HTTP Service \n(path, default=www)")
            ("queue", po::value<bool>(&queue_configuration_->enabled), "The Queue Service module is enabled \n(boolean, on|off, default=off)")
            ("queue_threads", po::value<int>(&queue_configuration_->threads), "The number of threads used by Queue Service \n(quantity, default=3)")
    ;

    po::variables_map options;
    po::store(po::parse_command_line(argc, argv, options_description_), options);
    po::notify(options);

    if (options.count("help")) {
        std::cout << options_description_ << std::endl;
        return 1;
    }

    if (options.count("run")) {

        if (http_configuration_->enabled) {
            std::thread http_thread_([&] {
                tcp::endpoint http_endpoint_;

                if (options.count("http_port")) {
                    http_endpoint_ = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(http_configuration_->port) };
                } else {
                    http_endpoint_ = { boost::asio::ip::tcp::v4(), boost::asio::ip::port_type(8080) };
                }

                boost::asio::io_context http_ioc_;

                ssl::context http_ctx_ { ssl::context::tlsv12 };
                ssl_certificates(http_ctx_);

                std::shared_ptr<std::string> http_doc_root_;

                if (options.count("http_directory")) {
                    http_doc_root_ = std::make_shared<std::string>(http_configuration_->directory);
                } else {
                    http_doc_root_ = std::make_shared<std::string>("www");
                }

                if (!options.count("http_read_timeout")) {
                    http_configuration_->read_timeout = 30;
                }

                if (!options.count("http_write_timeout")) {
                    http_configuration_->write_timeout = 30;
                }

                if (!options.count("http_handshake_timeout")) {
                    http_configuration_->handshake_timeout = 30;
                }

                std::make_shared<http_listener>(http_ioc_, http_ctx_, http_endpoint_, http_doc_root_, http_configuration_)->run();

                std::vector<std::thread> http_threads_;

                if (options.count("http_threads")) {
                    http_threads_.reserve(http_configuration_->threads);
                    for(auto i = http_configuration_->threads; i > 0; --i)
                        http_threads_.emplace_back([&http_ioc_]{ http_ioc_.run(); });
                } else {
                    http_threads_.reserve(3);
                    for(auto i = 3; i > 0; --i)
                        http_threads_.emplace_back([&http_ioc_]{ http_ioc_.run(); });
                }
                http_ioc_.run();
            });
            http_thread_.detach();
        }

        boost::lockfree::queue<queue_task *> queue_(1024);
        if (options.count("queue")) {
            boost::atomic_int queue_producer_count_(0);
            boost::atomic_int queue_consumer_count_(0);

            auto queue_consumer_ = [&] {
                queue_task * task_;
                while (queue_.pop(task_)) {
                    task_->run();
                }
            };

            std::thread queue_thread_([&] {
                while(true) {
                    if (queue_.empty()) {
                        usleep(100000);
                    } else {
                        boost::atomic<bool> queue_iteration_running_ (true);
                        boost::thread_group queue_consumer_threads;

                        if (options.count("queue_threads")) {
                            for (int i = 0; i != queue_configuration_->threads; ++i)
                                queue_consumer_threads.create_thread(queue_consumer_);
                        } else {
                            for (int i = 0; i != 3; ++i)
                                queue_consumer_threads.create_thread(queue_consumer_);
                        }
                        queue_consumer_threads.join_all();
                    }
                }
            });

            for (int i = 0; i != 5; ++i) {
                std::thread job_([] {
                    std::cout << "Awesome" << std::endl;
                });
                auto * heartbeat_task_ = new queue_task(std::move(job_));
                queue_.push(heartbeat_task_);
            }
            std::cout << "Joined" << std::endl;
            queue_thread_.detach();
        }

        std::thread run_thread_([&] {
            while(true) {
                sleep(5);
            }
        });

        run_thread_.join();
    }

    return 0;
}

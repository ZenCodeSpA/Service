#ifndef SYSTEM_HTTP_CONFIGURATION_H
#define SYSTEM_HTTP_CONFIGURATION_H


struct http_configuration {
    bool enabled;
    int port;
    int threads;
    long read_timeout;
    long write_timeout;
    long handshake_timeout;
    std::string directory;
};

#endif //SYSTEM_HTTP_CONFIGURATION_H

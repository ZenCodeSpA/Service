#ifndef SYSTEM_QUEUE_CONFIGURATION_H
#define SYSTEM_QUEUE_CONFIGURATION_H


struct queue_configuration {
    bool enabled;
    int threads;
    long long wait_timeout;
};

#endif //SYSTEM_QUEUE_CONFIGURATION_H

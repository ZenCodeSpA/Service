#include <thread>

#ifndef SYSTEM_QUEUE_TASK_H
#define SYSTEM_QUEUE_TASK_H

class queue_task {
    std::thread job_;
public:
    explicit queue_task(std::thread job): job_(std::move(job)) {}
    void run() {
        job_.join();
    }
    ~queue_task() = default;
};

#endif //SYSTEM_QUEUE_TASK_H

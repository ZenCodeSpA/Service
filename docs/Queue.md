# Queue

## Overview

```c++
#include "services/queue_task.h"

int queue_task_limit = 1024;

boost::lockfree::queue<queue_task *> queue_(queue_task_limit);

boost::json::value value {
    {"hello", "world"}
};

std::thread job([&] (boost::json::value & value) {
    std::cout <<
        value.at("hello").as_string()
        << std::endl;

}, std::move(value));

auto * task = new queue_task(std::move(job));

queue_.push(task);
```
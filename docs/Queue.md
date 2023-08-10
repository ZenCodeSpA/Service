# Queue

## Definition

```c++
#include "services/queue_task.h"

std::thread job([&] { });
auto * task = new queue_task(std::move(job));
```

### Dispatch

```c++
#include "services/queue_task.h"

queue_.push(task);
```
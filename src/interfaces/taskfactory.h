#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include "tasks/task.h"

//NOTE: Missing doc on all methods

class TaskFactory {
public:
    virtual Task *task() const = 0;
};

#endif // TASKFACTORY_H

#ifndef LOOP_THREAD_H_
#define LOOP_THREAD_H_

#include <thread>
#include <atomic>

#include "../../../../SmtObj.h"

class LoopThread
{
private:
    SmtObj<std::thread> _loop_thread;
    std::atomic<bool> _run_flag = { false };

    void (*_loop_action)(void *) = nullptr;
    void *_action_parameters = nullptr;

    static void _ActionLoop(LoopThread *loop_thread);

public:
    LoopThread(void (*function)(void *) = nullptr, void *func_parameters = nullptr);
    LoopThread(const LoopThread &) = delete;
    ~LoopThread();

    void Start();
    void Stop();
};

#endif /* LOOP_THREAD_H_ */

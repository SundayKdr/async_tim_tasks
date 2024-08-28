#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "embedded_hw_utils/utils/task_callback.hpp"

namespace async_tim_task_impl{

using CallBackT = utils::task::CallBack<>;

constexpr std::size_t kTick_freq_ = 1;

class AsyncTask{
public:
    AsyncTask() = default;

    explicit AsyncTask(CallBackT&& cb, std::size_t delay, bool suspended = false)
        : interval_(delay)
        , inited_(true)
        , handler_(cb)
    {
        if(!suspended)
            Enable();
    }

    [[gnu::always_inline]] void TickHandle(){
        count_ += kTick_freq_;
    }

    [[gnu::always_inline]] void Poll(){
        if(!disabled_ && count_ >= interval_){
            handler_();
            count_ = 0;
        }
    }

    void SetDelay(unsigned int delay){
        interval_ = delay;
    }

    void Enable(){
        disabled_ = false;
    }

    void Restart(){
        count_ = 0;
        Enable();
    }

    void Disable(){
        disabled_ = true;
    }

    [[nodiscard]] bool IsInited() const{
        return inited_;
    }

    void Reset(){
        disabled_ = true;
        inited_ = true;
        count_ = 0;
    }
private:
    std::size_t count_{0};
    std::size_t interval_{0};
    bool inited_ {false};
    bool disabled_ {true};
    CallBackT handler_;
};

}// namespace async_tim_task
#pragma once

#include <optional>
#include <limits>

#include "async_tim_tasks/impl/task.hpp"

namespace async_tim_task_impl{

template<std::size_t pool_size>
struct TaskPool{
    using ConverterFt = uint32_t(*)(float);
    ~TaskPool() = default;
    TaskPool(TaskPool&) = delete;
    TaskPool(TaskPool&&) = delete;
    TaskPool& operator = (TaskPool &) = delete;
    TaskPool& operator = (TaskPool const &) = delete;

    static auto& GetPool(){
        static TaskPool task_pool{};
        return task_pool;
    }

    void SetUp(ConverterFt converter, const std::function<void()>& starter){
        converter_ = converter;
        starter();
    }

    constexpr int PlaceToPool(CallBackT&& cb, float Hz = std::numeric_limits<float>::max(), bool suspended = false){
        assert(converter_ != nullptr);
        int idx = -1;
        for(std::size_t i = 0; i < pool_size; i++){
            if(!pool_[i].IsInited()){
                pool_[i] = AsyncTask{std::forward<CallBackT>(cb), converter_(Hz), suspended};
                idx = i;
                current_pool_size_++;
                break;
            }
        }
        return idx;
    }

    bool RemoveFromPool(unsigned short idx){
        if(idx < current_pool_size_){
            pool_[idx].Reset();
            current_pool_size_--;
            return true;
        }
        return false;
    }

    bool StopTask(unsigned short idx){
        if(idx < current_pool_size_){
            pool_[idx].Disable();
            return true;
        }
        return false;
    }

    bool ResumeTask(unsigned short idx){
        if(idx < current_pool_size_){
            pool_[idx].Enable();
            return true;
        }
        return false;
    }

    bool ResetTask(unsigned short idx){
        if(idx >= pool_size)
            return false;
        pool_[idx].Reset();
        return true;
    }

    bool RestartTask(unsigned short idx){
        if(idx < current_pool_size_){
            pool_[idx].Restart();
            return true;
        }
        return false;
    }

    void ResetInterval(unsigned short idx, float Hz){
        if(idx < current_pool_size_)
            pool_[idx].ResetInterval(converter_(Hz));
    }

    [[gnu::always_inline]] void OnTimTick(){
        for(std::size_t i = 0; i < current_pool_size_; i++)
            pool_[i].TickHandle();
    }

    [[gnu::always_inline]] void Poll(){
        for(std::size_t i = 0; i < current_pool_size_; i++)
            pool_[i].Poll();
    }
private:
    TaskPool() = default;
    std::size_t current_pool_size_ {0};
    std::array<AsyncTask, pool_size> pool_;
    ConverterFt converter_{};
};

}// namespace async_tim_task
#pragma once

#include "async_tim_tasks/impl/task_pool.hpp"

namespace async_tim_task{

constexpr std::size_t k_pool_size = 30;

using TaskPool = async_tim_task_impl::TaskPool<k_pool_size>;

#define $AsyncTasksPoolSetUp(converter, starter) async_tim_task::TaskPool::GetPool().SetUp(converter, starter)
#define $AsyncTasksOnTim async_tim_task::TaskPool::GetPool().OnTimTick();
#define $AsyncTasksPoll async_tim_task::TaskPool::GetPool().Poll();

#define $RunAsync(...) $RunAsyncMacroChooser(__VA_ARGS__)(__VA_ARGS__)
#define $RunAsyncStatic(...) $RunAsyncStaticMacroChooser(__VA_ARGS__)(__VA_ARGS__)
#define $RegAsyncSuspended(...) $RegAsyncMacroChooser(__VA_ARGS__)(__VA_ARGS__)

#define $RemoveAsyncTask(n) async_tim_task::TaskPool::GetPool().RemoveFromPool(n)
#define $StopAsyncTask(n) async_tim_task::TaskPool::GetPool().StopTask(n)
#define $ResumeAsyncTask(n) async_tim_task::TaskPool::GetPool().ResumeTask(n)
#define $RestartAsyncTask(n) async_tim_task::TaskPool::GetPool().RestartTask(n)
#define $ResetAsyncTask(n) async_tim_task::TaskPool::GetPool().ResetTask(n)
#define $ResetAsyncTaskIntervalHz(task_n, hz) async_tim_task::TaskPool::GetPool().ResetInterval(task_n, hz)

#define $get_override(arg1, arg2, arg3, ...) arg3

#define $RunAsyncMacroChooser(...) $get_override(__VA_ARGS__, $RunAsync_hz, $RunAsync_quickest)
#define $RunAsync_quickest(expr) \
    async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT(this, [](void* context){    \
            auto self = static_cast<decltype(this)>(context);                                                       \
            self->expr;                                                                                             \
        }                                                                                                           \
    ))
#define $RunAsync_hz(expr, Hz)  \
    async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT(this, [](void* context){    \
            auto self = static_cast<decltype(this)>(context);                                                       \
            self->expr;                                                                                             \
        }                                                                                                           \
    ), Hz)

#define $RunAsyncStaticMacroChooser(...) \
        $get_override(__VA_ARGS__, $RunAsync_static_hz, $RunAsyncStatic_quickest)
#define $RunAsync_static_hz(expr, Hz)  \
        async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT([](void* context){expr;}), Hz)
#define $RunAsyncStatic_quickest(expr)  \
        async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT([](void* context){expr;}))

#define $RegAsyncMacroChooser(...) \
                             $get_override(__VA_ARGS__, $RegAsyncTask_suspended, $RegAsyncInitial_suspended_quickest)

#define $RegAsyncTask_suspended(expr, Hz)  \
    async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT(this, [](void* context){    \
            auto self = static_cast<decltype(this)>(context);                                                       \
            self->expr;                                                                                             \
        }                                                                                                           \
    ), Hz, true)

#define $RegAsyncInitial_suspended_quickest(expr)  \
    async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT(this, [](void* context){    \
            auto self = static_cast<decltype(this)>(context);                                                       \
            self->expr;                                                                                             \
        }                                                                                                           \
    ), true)

//    template<typename F>
//    auto RunAsync(F callback, float hz) {
//        return async_tim_task::TaskPool::GetPool().PlaceToPool(async_tim_task_impl::CallBackT(new F(std::move(callback)), [](void* context){
//            auto self = static_cast<F*>(context);
//            std::move(*self)();
//        }), hz);
//    }

}// namespace async_tim_task
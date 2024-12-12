## Async tasks for your cpp app in freestanding env
#### This library is for you, if you need to manage multiple tasks asynchronously in you cpp project running freestanding env.<br />
### **No dependencies**
Short brief how to set up and how to run async tasks in your code:
### How to set up:
#### 1. Place macro $AsyncTasksOnTim in interrupt handler for desired timer.
```cpp    
   //For example we will use tim1
   #define POOL_TIM TIM1

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if(htim->Instance == POOL_TIM) [[likely]]
            $AsyncTasksOnTim
    }
```           
#### 2. Place macro $AsyncTasksPoll in main loop
```cpp         
    void device_main_loop(){
        $AsyncTasksPoll
    }
```           
#### 3. Place macro $AsyncTasksPoolSetUp in init section of your app<br />
   1st argument lambda converting time in Hertz to delay **`[](float Hz)->uint32_t {... return delay;}`**<br />
   2nd argument lambda starting your timer **`[]->void { StartTim(tim); }`**<br />
   Example for stm32:
```cpp
   //we hide pointer to tim in PoolTim() in order to ease use
   auto PoolTim(){
       return &htim1;
   }
   
   //Here we use core func and structs (STM32) to set converting Hertz to delay for our tim
    void initDevice(){
        $AsyncTasksPoolSetUp(
                [](float hz)->uint32_t{
                     return (SystemCoreClock / (PoolTim()->Instance->PSC) / (PoolTim()->Instance->ARR) / hz);
                  },
                []{ HAL_TIM_Base_Start_IT(PoolTim()); }
                );
    }
```  
#### 4. You have an option to change number of tasks to run in async_tim_tasks.hpp file<br />
   Default is **30** tasks
```cpp   
   namespace async_tim_task{
      constexpr std::size_t k_pool_size = 30;
   }
```  
### How to place a task:
#### 1. To place a task in static context (e.g. in free function) <br />
1st argument your function call or any expression in {} <br />
2nd argument float: time in Hertz for this task to repeat <br />
return value is task N in pool (std::size_t) <br />
```cpp            
   //Here we call stm32 HAL toggle pin func 2 times a second
   auto task_n = $RunAsyncStatic( HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin), 2);
   //or 
   auto task_n = $RunAsyncStatic( {HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);}, 2);
```           
#### 2. To place a task within any environment with desired delay<br />
1st argument your function call <br />
2nd argument float -> time in Hertz for this task to repeat <br />
return value is task N in pool (std::size_t) <br />
```cpp            
   //Here we call Update func on instance 50 times a second
   auto task_n = $RunAsync( Update(), 50);
```           
#### 3. To place a task within any environment with maximum frequency<br />
1st argument your function call
return value is task N in pool (std::size_t)
```cpp 
   auto task_n = $RunAsync( Update() );
```  
#### 4. to stop a task<br />
use macro $StopAsyncTask<br />
1st argument is task N which was a return value for $RunAsync...<br />
```cpp 
   $StopAsyncTask(task_n);
```  
#### 5. To resume a task<br />
use macro $ResumeAsyncTask<br />
1st argument is task N -> which was a return value for $RunAsync...<br />
```cpp 
   $ResumeAsyncTask(task_n);
```  
#### 6. Also, you can<br />
```cpp
    // Restart a task with counter set to 0
    $RestartAsyncTask(task_n)
    // Reset a task means to stop a task and set counter to 0
    $ResetAsyncTask(task_n)
    // Reset time interval in Hertz for desired task
    $ResetAsyncTaskIntervalHz(task_n, hz)
```  
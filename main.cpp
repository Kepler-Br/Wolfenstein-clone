#include <iostream>
#include "main_loop.h"
#include "state_game.h"
#include "lookup_table.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

//std::condition_variable cv;
//bool alldata_consumed = true;
//std::mutex m;
//std::mutex done_mutex;
//int workers_done = 0;
//constexpr int total_workers = 10;

//void producer()
//{
//    while(true)
//    {
//        std::unique_lock<std::mutex> um(m);
//        cv.wait(um, [](){return workers_done < total_workers;});
//        std::cout << "producing" << std::endl;
//        um.unlock();
//        done_mutex.lock();
//        workers_done++;
//        done_mutex.unlock();
//        cv.notify_one();
//    }
//}

//void consumer()
//{
//    while(true)
//    {
//        std::unique_lock<std::mutex> um(m);
//        cv.wait(um, [](){return workers_done == total_workers;});
//        std::cout << "consuming..." << std::endl;
//        alldata_consumed = true;
//        um.unlock();
//        done_mutex.lock();
//        workers_done = 0;
//        done_mutex.unlock();
//        cv.notify_all();
//    }
//}




class Worker
{
private:
    std::condition_variable *new_task_cv;
    std::condition_variable *task_done;
    std::mutex *new_task_mutex;
    std::mutex *task_done_mutex;
    std::atomic<uint> *tasks_done;
    std::atomic<bool> is_running;
    uint max_threads;

public:
    Worker(std::condition_variable *new_task_cv, std::condition_variable *task_done,
           std::mutex *new_task_mutex, std::mutex *task_done_mutex, int max_threads,
           std::atomic<uint> *tasks_done)
    {
        this->new_task_cv = new_task_cv;
        this->task_done = task_done;
        this->new_task_mutex = new_task_mutex;
        this->task_done_mutex = task_done_mutex;
        this->tasks_done = tasks_done;
        this->max_threads = max_threads;
    }

    void run()
    {
        this->is_running.store(true);
        while(this->is_running.load())
        {
            std::unique_lock<std::mutex> unique_mutex(*this->new_task_mutex);
            this->new_task_cv->wait(unique_mutex, [this](){return !this->is_running.load() || this->tasks_done->load() < this->max_threads;});
        }
    }

    void stop()
    {
        this->is_running.store(false);
    }
};

int main()
{
//    std::condition_variable new_task_cv;
//    std::condition_variable task_done;
//    std::mutex new_task_mutex;
//    std::mutex task_done_mutex;
//    constexpr int max_threads = 1;
//    std::atomic<uint> tasks_done;
//    while(true)
//    {

//    }
//    Main_loop main_loop({320, 200});
    SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_DEBUG);
    Main_loop main_loop({800, 600});
    auto *game = new State_game(main_loop, main_loop.get_input_manager(), main_loop.get_sdl_instance());
    game->preload();
    main_loop.push_state((State_base *)game);
    try
    {
    main_loop.start();
    }
    catch (const std::runtime_error &e)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Runtime error",
                                 e.what(),
                                 NULL);
    }
    return 0;
}

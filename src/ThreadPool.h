#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <optional>


class ThreadPool
{
private:

    class Job {
        std::function<void()> funcy;
    public:
        template <typename ... Args_t>
        Job(std::function<void(Args_t...)>&& func, Args_t ... args) :
             funcy([func = std::forward<std::function<void(Args_t...)>>(func), 
                    args = std::make_tuple(std::forward<Args_t>(args) ...)]()
                    {
                        std::apply(func, args);
                    })
        {
        };

        void operator()()
        {
            funcy();
        }

    };


    std::queue<Job> jobs;
    std::mutex jobs_mutex;
    std::condition_variable jobs_cv;
    
    std::vector<std::thread> workers;
    bool isShuttingDown;
    

    void worker_thread()
    {
        while(true)
        {
            std::optional<Job> job(std::nullopt);
            {
                std::unique_lock<std::mutex> lock(jobs_mutex);
                jobs_cv.wait(lock, [&](){return isShuttingDown || !jobs.empty();});
                
                if(isShuttingDown && jobs.empty())
                    break;
                job = jobs.front();
                jobs.pop();
            }
            job.value()();
        }
    };

public:
    
    ThreadPool(unsigned int numThreads=std::thread::hardware_concurrency()) :
        isShuttingDown(false)
    {
        for(int i=0; i<numThreads; i++)
            workers.emplace_back([&](){worker_thread();});
    };

    ~ThreadPool()
    {
        isShuttingDown = true;
        jobs_cv.notify_all();

        for(auto& thread : workers)
        {
            if(thread.joinable())
                thread.join();
        }
    }

    ThreadPool(const ThreadPool& runner) = default;
    ThreadPool& operator=(const ThreadPool& runner) = default;
    ThreadPool(ThreadPool&& runner) = default;
    ThreadPool& operator=(ThreadPool&& runner) = default;

    template <typename... Args_t> 
    void addJob(std::function<void(Args_t...)>&& function, Args_t&&... args)
    {
        jobs.emplace(std::forward<decltype(function)>(function), std::forward<Args_t>(args)...);
        jobs_cv.notify_one();
    }
};
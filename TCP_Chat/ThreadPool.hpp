#undef Thread
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "Log.hpp"
#include "Mutex.hpp"
#include "TestPthread.hpp"
#include "Cond.hpp"

using namespace LogMoudle;
using namespace MutexModule;
using namespace ThreadModlue;
using namespace CondModule;
namespace ThreadPoolMoudle
{

    static const int defaultthreadnum = 5;
    template <class T>
    class ThreadPool
    {
    public:
        ThreadPool(int num = defaultthreadnum) : _num(num), _isrunning(false), _sleepnum(0)
        {
            for (int i = 0; i < _num; i++)
            {
                _threads.emplace_back(
                    [this]()
                    { ThreadTask(); });
            }
        }

        static ThreadPool<T> *GetInstance()
        {
            if (inc == nullptr)
            {
                LockGrund lockguard(_lock);

                LOG(LogLevel::DEBUG) << "获取单例....";
                if (inc == nullptr)
                {
                    LOG(LogLevel::DEBUG) << "首次使用单例, 创建之....";
                    inc = new ThreadPool<T>();
                    inc->Start();
                }
            }

            return inc;
        }
        void WakeUpAllThread()
        {
            if (_sleepnum)
            {
                _cond.Broadcast();
                LOG(LogLevel::INFO) << "唤醒所有线程";
            }
            return;
        }

        void WakeUpOneThread()
        {
            _cond.Signal();
            LOG(LogLevel::INFO) << "唤醒一个线程";
            return;
        }

        void Start()
        {
            if (_isrunning)
                return;
            _isrunning = true;
            for (auto &thread : _threads)
            {
                thread.Start();
                LOG(LogLevel::INFO) << "start new thread success: " << thread.Name();
            }
            return;
        }

        void Stop()
        {
            if (_isrunning)
            {
                _isrunning = false;
                WakeUpAllThread();
            }
            return;
        }
        void Join()
        {
            for (auto &threads : _threads)
            {
                threads.Join();
                LOG(LogLevel::INFO) << "Join thread success: " << threads.Name();
            }
        }

        void ThreadTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            T t;
            while (true)
            {
                LockGrund lock(_mutex);
                if (_isrunning && _taskq.empty())
                {
                    _sleepnum++;
                    _cond.Wait(_mutex);
                    _sleepnum--;
                }

                if (!_isrunning && _taskq.empty())
                {
                    LOG(LogLevel::INFO) << name << " 退出了, 线程池退出&&任务队列为空";
                    break;
                }

                t = _taskq.front();
                _taskq.pop();
                t();
            }


            return;
        }

        bool Enqueue(const T &in)
        {
            if (_isrunning)
            {
                LockGrund lock(_mutex);
                _taskq.push(in);
                // if (_taskq.size() == _sleepnum)
                if (!_taskq.empty()&& _sleepnum)
                {
                    WakeUpOneThread();
                }
                return true;
            }

            return false;
        }

    private:
        std::vector<Thread> _threads;
        std::queue<T> _taskq;

        int _num; // 线程数量
        Mutex _mutex;
        Cond _cond;
        bool _isrunning;
        int _sleepnum;
        static ThreadPool<T> *inc;
        static Mutex _lock;
    };
    template <class T>
    ThreadPool<T> *ThreadPool<T>::inc = nullptr;
    template <typename T>
    Mutex ThreadPool<T>::_lock;

}
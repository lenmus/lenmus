//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2013 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#ifndef __LOMSE_THREADS_H__
#define __LOMSE_THREADS_H__

#include <string>
#include <iostream>
using namespace std;

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

namespace lomse
{

//---------------------------------------------------------------------------------------
//forward declarations
class LibraryScope;


class ThreadPool;

//---------------------------------------------------------------------------------------
// our worker thread objects
class Worker
{
public:
    Worker(ThreadPool &s) : pool(s) {}
    void operator()();

private:
    ThreadPool &pool;
};

//---------------------------------------------------------------------------------------
// the actual thread pool
class ThreadPool
{
private:
    // need to keep track of threads so we can join them
    std::vector< boost::thread* > workers;

    // the io_service we are wrapping
    boost::asio::io_service service;
    boost::asio::io_service::work working;
    friend class Worker;

public:
    ThreadPool(size_t);
    template<class F>
    void enqueue(F f);
    ~ThreadPool();
};


//---------------------------------------------------------------------------------------
// implementation
//---------------------------------------------------------------------------------------

// all the workers do is execute the io_service
void Worker::operator()() { pool.service.run(); }


// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads) : working(service)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back( LOMSE_NEW boost::thread(Worker(*this)) );
}

// add new work item to the pool
template<class F>
void ThreadPool::enqueue(F f)
{
    service.post(f);
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    service.stop();
    for(size_t i = 0;i<workers.size();++i)
        workers[i]->join();
}


}   //namespace lomse

#endif      //__LOMSE_THREADS_H__

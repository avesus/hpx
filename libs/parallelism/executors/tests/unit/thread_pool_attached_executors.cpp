//  Copyright (c) 2007-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/executors/thread_pool_attached_executors.hpp>
#include <hpx/local/init.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
hpx::thread::id test(int passed_through)
{
    HPX_TEST_EQ(passed_through, 42);
    return hpx::this_thread::get_id();
}

template <typename Executor>
void test_sync(Executor& exec)
{
    HPX_TEST(hpx::parallel::execution::sync_execute(exec, &test, 42) !=
        hpx::this_thread::get_id());
}

template <typename Executor>
void test_async(Executor& exec)
{
    HPX_TEST(hpx::parallel::execution::async_execute(exec, &test, 42).get() !=
        hpx::this_thread::get_id());
}

///////////////////////////////////////////////////////////////////////////////
hpx::thread::id test_f(hpx::future<void> f, int passed_through)
{
    HPX_TEST(f.is_ready());    // make sure, future is ready

    f.get();    // propagate exceptions

    HPX_TEST_EQ(passed_through, 42);
    return hpx::this_thread::get_id();
}

template <typename Executor>
void test_then(Executor& exec)
{
    hpx::future<void> f = hpx::make_ready_future();

    HPX_TEST(
        hpx::parallel::execution::then_execute(exec, &test_f, f, 42).get() !=
        hpx::this_thread::get_id());
}

///////////////////////////////////////////////////////////////////////////////
void bulk_test(int, hpx::thread::id tid, int passed_through)    //-V813
{
    HPX_TEST_NEQ(tid, hpx::this_thread::get_id());
    HPX_TEST_EQ(passed_through, 42);
}

template <typename Executor>
void test_bulk_sync(Executor& exec)
{
    hpx::thread::id tid = hpx::this_thread::get_id();

    std::vector<int> v(107);
    std::iota(std::begin(v), std::end(v), std::rand());

    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;

    hpx::parallel::execution::bulk_sync_execute(
        exec, hpx::util::bind(&bulk_test, _1, tid, _2), v, 42);
    hpx::parallel::execution::bulk_sync_execute(exec, &bulk_test, v, tid, 42);
}

template <typename Executor>
void test_bulk_async(Executor& exec)
{
    hpx::thread::id tid = hpx::this_thread::get_id();

    std::vector<int> v(107);
    std::iota(std::begin(v), std::end(v), std::rand());

    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;

    hpx::when_all(hpx::parallel::execution::bulk_async_execute(
                      exec, hpx::util::bind(&bulk_test, _1, tid, _2), v, 42))
        .get();
    hpx::when_all(hpx::parallel::execution::bulk_async_execute(
                      exec, &bulk_test, v, tid, 42))
        .get();
}

///////////////////////////////////////////////////////////////////////////////
void bulk_test_f(int, hpx::shared_future<void> f, hpx::thread::id tid,
    int passed_through)    //-V813
{
    HPX_TEST(f.is_ready());    // make sure, future is ready

    f.get();    // propagate exceptions

    HPX_TEST_NEQ(tid, hpx::this_thread::get_id());
    HPX_TEST_EQ(passed_through, 42);
}

template <typename Executor>
void test_bulk_then(Executor& exec)
{
    hpx::thread::id tid = hpx::this_thread::get_id();

    std::vector<int> v(107);
    std::iota(std::begin(v), std::end(v), std::rand());

    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;
    using hpx::util::placeholders::_3;

    hpx::shared_future<void> f = hpx::make_ready_future();

    hpx::parallel::execution::bulk_then_execute(
        exec, hpx::util::bind(&bulk_test_f, _1, _2, tid, _3), v, f, 42)
        .get();
    hpx::parallel::execution::bulk_then_execute(
        exec, &bulk_test_f, v, f, tid, 42)
        .get();
}

///////////////////////////////////////////////////////////////////////////////
template <typename Executor>
void test_thread_pool_attached_executor(Executor& exec)
{
    test_sync(exec);
    test_async(exec);
    test_then(exec);
    test_bulk_sync(exec);
    test_bulk_async(exec);
    test_bulk_then(exec);
}

int hpx_main()
{
    using namespace hpx::parallel;

    std::size_t num_threads = hpx::get_os_thread_count();

    {
        execution::static_queue_attached_executor exec(0, num_threads);
        test_thread_pool_attached_executor(exec);
    }

    {
        execution::local_priority_queue_attached_executor exec(0, num_threads);
        test_thread_pool_attached_executor(exec);
    }

    {
        execution::static_priority_queue_attached_executor exec(0, num_threads);
        test_thread_pool_attached_executor(exec);
    }

    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on one core (the executors create more
    // threads)
    std::vector<std::string> const cfg = {"hpx.os_threads=1"};

    // Initialize and run HPX
    hpx::local::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ_MSG(hpx::local::init(hpx_main, argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}

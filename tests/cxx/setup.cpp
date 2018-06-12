#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <csignal>
#include <iostream>

#include <execinfo.h>

using namespace std;
using namespace testing;


void handler(int sig)
{
    void *array[20];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 20);

    // print out all the frames to stderr
    cerr << "Error: signal " << sig << ":" << endl;
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

struct global_environment : Environment
{
    void SetUp() override
    {
        signal(SIGSEGV, handler);
    }
};

::testing::Environment* const global_env = ::testing::AddGlobalTestEnvironment(new global_environment);

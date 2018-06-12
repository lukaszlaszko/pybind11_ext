#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <csignal>
#include <execinfo.h>

using namespace std;
using namespace testing;


void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
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

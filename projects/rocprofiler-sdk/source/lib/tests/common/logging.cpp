// MIT License
//
// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "lib/common/logging.hpp"

#include <gtest/gtest.h>

#include <unistd.h>
#include <cstdio>
#include <cstring>

TEST(common, logging_active_after_init)
{
    // init_logging is called at file scope in environment.cpp with TEST_LOG_LEVEL=info,
    // so logging_active should be true by the time tests run.
    EXPECT_TRUE(rocprofiler::common::logging_active().load());
}

TEST(common, logging_guard_drops_info_when_inactive)
{
    // Temporarily disable logging
    rocprofiler::common::logging_active() = false;

    // ROCP_INFO should be a no-op (no crash, no output)
    ROCP_INFO << "this message should be silently dropped";

    // Re-enable
    rocprofiler::common::logging_active() = true;
}

TEST(common, logging_guard_drops_warning_when_inactive)
{
    rocprofiler::common::logging_active() = false;

    ROCP_WARNING << "this warning should be silently dropped";

    rocprofiler::common::logging_active() = true;
}

TEST(common, stderr_stream_writes_to_stderr)
{
    // Capture stderr output
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    int saved_stderr = dup(STDERR_FILENO);
    dup2(pipefd[1], STDERR_FILENO);

    {
        auto s = rocprofiler::common::StderrStream("test.cpp", 42);
        s.stream() << "test message " << 123;
    }  // destructor writes to stderr

    // Flush and restore stderr
    fflush(stderr);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
    close(pipefd[1]);

    // Read captured output
    char    buf[512] = {};
    ssize_t n        = read(pipefd[0], buf, sizeof(buf) - 1);
    close(pipefd[0]);

    ASSERT_GT(n, 0);
    buf[n] = '\0';

    EXPECT_NE(strstr(buf, "test message 123"), nullptr)
        << "Expected 'test message 123' in stderr output: " << buf;
    EXPECT_NE(strstr(buf, "test.cpp"), nullptr) << "Expected 'test.cpp' in stderr output: " << buf;
    EXPECT_NE(strstr(buf, "42"), nullptr) << "Expected '42' in stderr output: " << buf;
}

TEST(common, rocp_error_falls_back_to_stderr_when_inactive)
{
    // Capture stderr
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    int saved_stderr = dup(STDERR_FILENO);
    dup2(pipefd[1], STDERR_FILENO);

    rocprofiler::common::logging_active() = false;

    ROCP_ERROR << "fallback error message";

    rocprofiler::common::logging_active() = true;

    fflush(stderr);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
    close(pipefd[1]);

    char    buf[512] = {};
    ssize_t n        = read(pipefd[0], buf, sizeof(buf) - 1);
    close(pipefd[0]);

    ASSERT_GT(n, 0);
    buf[n] = '\0';

    EXPECT_NE(strstr(buf, "fallback error message"), nullptr)
        << "Expected 'fallback error message' in stderr output: " << buf;
}

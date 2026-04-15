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

TEST(common, logging_guard_drops_error_when_inactive)
{
    rocprofiler::common::logging_active() = false;

    // ROCP_ERROR should be a no-op (no crash, no output)
    ROCP_ERROR << "this error should be silently dropped";

    rocprofiler::common::logging_active() = true;
}

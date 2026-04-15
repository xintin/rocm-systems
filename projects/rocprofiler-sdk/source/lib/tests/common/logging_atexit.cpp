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

/// Standalone test binary that verifies ROCP_* macros do not crash during atexit.
/// The test passes if the process exits cleanly (exit code 0, no signals).

#include "lib/common/logging.hpp"

#include <cstdlib>

namespace
{
void
atexit_logging_handler()
{
    // Simulate the scenario where logging is called during atexit after
    // logging_active has been set to false (as happens in the SDK's atexit handler).
    rocprofiler::common::logging_active() = false;

    // These must not crash — all are silently dropped when logging_active is false:
    ROCP_INFO << "info during atexit (should be dropped)";
    ROCP_WARNING << "warning during atexit (should be dropped)";
    ROCP_ERROR << "error during atexit (should be dropped)";
    ROCP_TRACE << "trace during atexit (should be dropped)";
}
}  // namespace

int
main()
{
    rocprofiler::common::init_logging("TEST");
    std::atexit(atexit_logging_handler);

    ROCP_INFO << "normal logging works";
    ROCP_ERROR << "normal error logging works";

    return 0;
}

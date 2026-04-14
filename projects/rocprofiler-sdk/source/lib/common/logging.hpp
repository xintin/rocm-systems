// MIT License
//
// Copyright (c) 2023-2025 Advanced Micro Devices, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "lib/common/defines.hpp"

#include <glog/logging.h>

#include <fmt/format.h>  // usually used in conjunction with logging
#include <fmt/ranges.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#define ROCP_LOG_LEVEL_TRACE   4
#define ROCP_LOG_LEVEL_INFO    3
#define ROCP_LOG_LEVEL_WARNING 2
#define ROCP_LOG_LEVEL_ERROR   1
#define ROCP_LOG_LEVEL_NONE    0

// clang-format off

// Silently dropped when logging is inactive (during teardown)
#define ROCP_TRACE   LOG_IF(INFO, VLOG_IS_ON(ROCP_LOG_LEVEL_TRACE) && ::rocprofiler::common::logging_active())
#define ROCP_INFO    LOG_IF(INFO, ::rocprofiler::common::logging_active())
#define ROCP_WARNING LOG_IF(WARNING, ::rocprofiler::common::logging_active())

// Falls back to stderr when logging is inactive
#define ROCP_ERROR                                                                             \
    (::rocprofiler::common::logging_active()                                                   \
         ? google::LogMessage(__FILE__, __LINE__, google::GLOG_ERROR).stream()                  \
         : ::rocprofiler::common::StderrStream(__FILE__, __LINE__).stream())

// Always active — fatal errors should always be reported
#define ROCP_FATAL  LOG(FATAL)
#define ROCP_DFATAL DLOG(FATAL)

// Conditional variants
#define ROCP_TRACE_IF(CONDITION)   LOG_IF(INFO, VLOG_IS_ON(ROCP_LOG_LEVEL_TRACE) && (CONDITION) && ::rocprofiler::common::logging_active())
#define ROCP_INFO_IF(CONDITION)    LOG_IF(INFO, (CONDITION) && ::rocprofiler::common::logging_active())
#define ROCP_WARNING_IF(CONDITION) LOG_IF(WARNING, (CONDITION) && ::rocprofiler::common::logging_active())
#define ROCP_ERROR_IF(CONDITION)                                                               \
    (!(CONDITION)                                                                              \
         ? ::rocprofiler::common::NullStream::instance()                                       \
         : ::rocprofiler::common::logging_active()                                             \
               ? google::LogMessage(__FILE__, __LINE__, google::GLOG_ERROR).stream()            \
               : ::rocprofiler::common::StderrStream(__FILE__, __LINE__).stream())
#define ROCP_FATAL_IF(CONDITION)   LOG_IF(FATAL, (CONDITION))
#define ROCP_DFATAL_IF(CONDITION)  DLOG_IF(FATAL, (CONDITION))

// clang-format on

#if defined(ROCPROFILER_CI)
#    define ROCP_CI_LOG_IF(NON_CI_LEVEL, ...) ROCP_FATAL_IF(__VA_ARGS__)
#    define ROCP_CI_LOG(NON_CI_LEVEL, ...)    ROCP_FATAL
#else
#    define ROCP_CI_LOG_IF(NON_CI_LEVEL, ...) ROCP_##NON_CI_LEVEL##_IF(__VA_ARGS__)
#    define ROCP_CI_LOG(NON_CI_LEVEL, ...)    ROCP_##NON_CI_LEVEL
#endif

namespace rocprofiler
{
namespace common
{
struct logging_config
{
    bool        install_failure_handler = false;
    bool        logtostderr             = true;
    bool        alsologtostderr         = false;
    bool        logdir_gitignore        = false;  // add .gitignore to logdir
    int32_t     loglevel                = google::WARNING;
    int32_t     vlog_level              = ROCP_LOG_LEVEL_WARNING;
    std::string vlog_modules            = {};
    std::string name                    = {};
    std::string logdir                  = {};
};

void
init_logging(std::string_view env_prefix, logging_config cfg = logging_config{});

void
update_logging(const logging_config& cfg);

/// Returns true when glog is initialized and safe to use.
/// Returns false before init_logging() and after the atexit handler runs.
std::atomic<bool>&
logging_active();

/// Lightweight stream that writes to stderr on destruction.
/// Used as a fallback for ROCP_ERROR when glog is not available.
class StderrStream
{
    std::ostringstream oss_;
    const char*        file_;
    int                line_;

public:
    StderrStream(const char* file, int line)
    : file_(file)
    , line_(line)
    {}

    ~StderrStream()
    {
        auto s = oss_.str();
        if(!s.empty()) fprintf(stderr, "[rocprofiler][%s:%d] %s\n", file_, line_, s.c_str());
    }

    std::ostringstream& stream() { return oss_; }
};

/// A no-op stream that discards all output. Used for disabled conditional macros.
class NullStream
{
    std::ostringstream oss_;

public:
    static std::ostringstream& instance()
    {
        static thread_local std::ostringstream s;
        s.str("");
        return s;
    }
};

}  // namespace common
}  // namespace rocprofiler

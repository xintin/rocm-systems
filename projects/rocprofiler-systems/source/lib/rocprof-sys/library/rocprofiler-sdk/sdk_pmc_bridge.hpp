// Copyright (c) Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT

#pragma once

#include <rocprofiler-sdk/fwd.h>
#include <rocprofiler-sdk/rocprofiler.h>

#include <cstddef>
#include <vector>

namespace rocprofsys::rocprofiler_sdk
{

/**
 * @brief Per-agent info stored by tool_init for the SDK PMC collector.
 */
struct sdk_pmc_agent_info
{
    rocprofiler_agent_id_t          agent_id       = {};
    rocprofiler_profile_config_id_t profile_config = {};
    size_t                          device_index   = 0;
};

/**
 * @brief Thread-safe singleton bridge between tool_init and the PMC sampler.
 *
 * tool_init() (in rocprofiler-sdk.cpp) creates the rocprofiler-sdk context, buffer,
 * and per-agent profile configurations for the device_counting_service. The PMC
 * sampler initializes later and needs access to these handles.
 *
 * This bridge is a passive data store — it owns no lifecycle. The rocprofiler-sdk
 * context/buffer are destroyed by tool_fini; the PMC sampler only reads from them.
 */
struct sdk_pmc_bridge
{
    rocprofiler_context_id_t        context     = { 0 };
    rocprofiler_buffer_id_t         buffer      = { 0 };
    std::vector<sdk_pmc_agent_info> agents      = {};
    bool                            initialized = false;

    /**
     * @brief Meyer's singleton accessor.
     * @return Reference to the global bridge instance.
     */
    static sdk_pmc_bridge& instance()
    {
        static sdk_pmc_bridge s_instance;
        return s_instance;
    }

    sdk_pmc_bridge(const sdk_pmc_bridge&)            = delete;
    sdk_pmc_bridge& operator=(const sdk_pmc_bridge&) = delete;
    sdk_pmc_bridge(sdk_pmc_bridge&&)                 = delete;
    sdk_pmc_bridge& operator=(sdk_pmc_bridge&&)      = delete;

private:
    sdk_pmc_bridge()  = default;
    ~sdk_pmc_bridge() = default;
};

}  // namespace rocprofsys::rocprofiler_sdk

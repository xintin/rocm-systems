// Copyright (c) Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT

#pragma once

#include "library/pmc/common/types.hpp"
#include "library/pmc/device_providers/rocprofiler_sdk/drivers/driver.hpp"
#include "library/rocprofiler-sdk/sdk_pmc_bridge.hpp"
#include "logger/debug.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace rocprofsys::pmc::device_providers::rocprofiler_sdk
{

/**
 * @brief Rocprofiler-SDK device provider for GPU hardware counter sampling.
 *
 * This provider reads pre-configured context, buffer, and per-agent profile configs
 * from the sdk_pmc_bridge singleton (populated by tool_init in rocprofiler-sdk.cpp).
 * It creates device objects that can poll counters via the device_counting_service.
 *
 * Unlike the AMD SMI provider, this class does NOT manage library init/shutdown —
 * the rocprofiler-sdk lifecycle is owned by rocprofiler-sdk.cpp.
 *
 * @tparam DriverFactory Factory for creating rocprofiler-sdk driver instances.
 */
template <typename DriverFactory>
class provider
{
public:
    using driver_t = typename DriverFactory::driver_t;

    /**
     * @brief Construct a provider by reading from the sdk_pmc_bridge.
     *
     * @throws std::runtime_error If the bridge has not been initialized by tool_init.
     */
    provider()
    : m_driver_api(DriverFactory::create_driver())
    {
        const auto& bridge = rocprofsys::rocprofiler_sdk::sdk_pmc_bridge::instance();
        if(!bridge.initialized)
        {
            throw std::runtime_error(
                "sdk_pmc_bridge not initialized. "
                "Ensure tool_init() configured the device_counting_service.");
        }

        // Start the SDK PMC context so that sampling calls succeed
        auto status = m_driver_api->start_context(bridge.context);
        if(status != ROCPROFILER_STATUS_SUCCESS)
        {
            LOG_WARNING("Failed to start context {} (status={})", bridge.context.handle,
                        static_cast<int>(status));
        }
        else
        {
            LOG_DEBUG("Started context {}", bridge.context.handle);
        }
    }

    ~provider() noexcept = default;

    provider(const provider&)            = delete;
    provider& operator=(const provider&) = delete;

    provider(provider&& other) noexcept
    : m_driver_api(std::move(other.m_driver_api))
    {
        other.m_driver_api.reset();
    }

    provider& operator=(provider&& other) noexcept
    {
        if(this != &other)
        {
            m_driver_api = std::move(other.m_driver_api);
            other.m_driver_api.reset();
        }
        return *this;
    }

    /**
     * @brief Enumerate devices of a specific type from the bridge.
     *
     * Creates Device objects from the per-agent info stored in the bridge.
     * Each device holds references to the shared context and its profile config.
     *
     * @tparam Device The device type to create.
     * @param type Device type to enumerate (only GPU is supported).
     * @return Vector of shared pointers to device objects.
     */
    template <typename Device>
    [[nodiscard]] std::vector<std::shared_ptr<Device>> get_devices(device_type type)
    {
        if(type != device_type::GPU)
        {
            return {};
        }

        const auto& bridge = rocprofsys::rocprofiler_sdk::sdk_pmc_bridge::instance();

        std::vector<std::shared_ptr<Device>> devices;
        devices.reserve(bridge.agents.size());

        for(const auto& agent_info : bridge.agents)
        {
            devices.push_back(std::make_shared<Device>(
                m_driver_api, bridge.context, agent_info.agent_id,
                agent_info.profile_config, agent_info.device_index));
        }

        return devices;
    }

    /**
     * @brief Shutdown the provider (no-op).
     *
     * The rocprofiler-sdk context lifecycle is managed by rocprofiler-sdk.cpp,
     * not by this provider.
     */
    void shutdown() {}

private:
    std::shared_ptr<typename DriverFactory::driver_t> m_driver_api;
};

/**
 * @brief Factory for creating rocprofiler-sdk provider instances.
 *
 * @tparam DriverFactory Factory type for creating driver instances.
 */
template <typename DriverFactory>
struct provider_factory
{
    using provider_t = provider<DriverFactory>;

    static std::shared_ptr<provider_t> create() { return std::make_shared<provider_t>(); }
};

}  // namespace rocprofsys::pmc::device_providers::rocprofiler_sdk

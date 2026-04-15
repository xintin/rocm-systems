// Copyright (c) Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <rocprofiler-sdk/buffer.h>
#include <rocprofiler-sdk/device_counting_service.h>
#include <rocprofiler-sdk/fwd.h>
#include <rocprofiler-sdk/rocprofiler.h>

namespace rocprofsys::pmc::drivers::rocprofiler_sdk
{

/**
 * @brief Thin wrapper around rocprofiler-sdk C APIs for device counting service.
 *
 * This struct provides static methods that directly forward to the rocprofiler-sdk
 * library. It serves as an abstraction layer that can be mocked in tests through
 * the driver_factory, following the same pattern as drivers::amd_smi::driver.
 */
struct driver
{
    /**
     * @brief Sample device counting service counters (synchronous poll).
     */
    static rocprofiler_status_t sample_device_counting_service(
        rocprofiler_context_id_t context, rocprofiler_user_data_t user_data,
        rocprofiler_counter_flag_t flags, rocprofiler_counter_record_t* output_records,
        size_t* record_count)
    {
        return rocprofiler_sample_device_counting_service(context, user_data, flags,
                                                          output_records, record_count);
    }

    /**
     * @brief Create a rocprofiler context.
     */
    static rocprofiler_status_t create_context(rocprofiler_context_id_t* context)
    {
        return rocprofiler_create_context(context);
    }

    /**
     * @brief Create a buffer for device counting service.
     */
    static rocprofiler_status_t create_buffer(rocprofiler_context_id_t context,
                                              size_t size, size_t watermark,
                                              rocprofiler_buffer_policy_t     policy,
                                              rocprofiler_buffer_tracing_cb_t callback,
                                              void*                    callback_data,
                                              rocprofiler_buffer_id_t* buffer_id)
    {
        return rocprofiler_create_buffer(context, size, watermark, policy, callback,
                                         callback_data, buffer_id);
    }

    /**
     * @brief Configure device counting service for a specific agent.
     */
    static rocprofiler_status_t configure_device_counting_service(
        rocprofiler_context_id_t context, rocprofiler_buffer_id_t buffer,
        rocprofiler_agent_id_t agent, rocprofiler_device_counting_service_cb_t callback,
        void* user_data)
    {
        return rocprofiler_configure_device_counting_service(context, buffer, agent,
                                                             callback, user_data);
    }

    /**
     * @brief Start a rocprofiler context.
     */
    static rocprofiler_status_t start_context(rocprofiler_context_id_t context)
    {
        return rocprofiler_start_context(context);
    }

    /**
     * @brief Stop a rocprofiler context.
     */
    static rocprofiler_status_t stop_context(rocprofiler_context_id_t context)
    {
        return rocprofiler_stop_context(context);
    }

    /**
     * @brief Create a profile config from counter IDs.
     */
    static rocprofiler_status_t create_profile_config(
        rocprofiler_agent_id_t agent, rocprofiler_counter_id_t* counters,
        size_t num_counters, rocprofiler_profile_config_id_t* profile)
    {
        return rocprofiler_create_profile_config(agent, counters, num_counters, profile);
    }

    /**
     * @brief Iterate over counters supported by an agent.
     */
    static rocprofiler_status_t iterate_agent_supported_counters(
        rocprofiler_agent_id_t agent, rocprofiler_available_counters_cb_t callback,
        void* user_data)
    {
        return rocprofiler_iterate_agent_supported_counters(agent, callback, user_data);
    }

    /**
     * @brief Query counter info (name, description, etc.).
     */
    static rocprofiler_status_t query_counter_info(
        rocprofiler_counter_id_t counter, rocprofiler_counter_info_version_id_t version,
        void* info)
    {
        return rocprofiler_query_counter_info(counter, version, info);
    }

    /**
     * @brief Query the counter ID from a record counter ID.
     */
    static rocprofiler_status_t query_record_counter_id(
        rocprofiler_counter_instance_id_t record_id, rocprofiler_counter_id_t* counter_id)
    {
        return rocprofiler_query_record_counter_id(record_id, counter_id);
    }

    /**
     * @brief Query dimension position for a record.
     */
    static rocprofiler_status_t query_record_dimension_position(
        rocprofiler_counter_instance_id_t  record_id,
        rocprofiler_counter_dimension_id_t dim_id, size_t* position)
    {
        return rocprofiler_query_record_dimension_position(record_id, dim_id, position);
    }

    /**
     * @brief Iterate over dimensions of a counter.
     */
    static rocprofiler_status_t iterate_counter_dimensions(
        rocprofiler_counter_id_t counter, rocprofiler_available_dimensions_cb_t callback,
        void* user_data)
    {
        return rocprofiler_iterate_counter_dimensions(counter, callback, user_data);
    }

    /**
     * @brief Query available agents.
     */
    static rocprofiler_status_t query_available_agents(
        rocprofiler_agent_version_t             version,
        rocprofiler_query_available_agents_cb_t callback, size_t agent_size,
        void* user_data)
    {
        return rocprofiler_query_available_agents(version, callback, agent_size,
                                                  user_data);
    }
};

/**
 * @brief Factory for creating driver instances.
 *
 * Enables dependency injection and allows substituting mock drivers in tests.
 */
struct driver_factory
{
    using driver_t = driver;

    static std::shared_ptr<driver_t> create_driver()
    {
        return std::make_shared<driver_t>();
    }
};

}  // namespace rocprofsys::pmc::drivers::rocprofiler_sdk

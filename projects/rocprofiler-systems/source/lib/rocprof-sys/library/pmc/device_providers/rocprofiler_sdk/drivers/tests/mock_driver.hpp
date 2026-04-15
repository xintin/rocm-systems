// Copyright (c) Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT

#pragma once

#include <gmock/gmock.h>

#include <rocprofiler-sdk/fwd.h>
#include <rocprofiler-sdk/rocprofiler.h>

#include <cstddef>

namespace rocprofsys::pmc::drivers::rocprofiler_sdk::testing
{

/**
 * @brief Mock implementation of rocprofiler-sdk driver for unit testing.
 *
 * Provides a Google Mock implementation of the SDK PMC driver interface,
 * enabling isolated testing of the device and provider classes without
 * requiring actual GPU hardware or the rocprofiler-sdk runtime.
 */
class mock_driver
{
public:
    MOCK_METHOD(rocprofiler_status_t, sample_device_counting_service,
                (rocprofiler_context_id_t context, rocprofiler_user_data_t user_data,
                 rocprofiler_counter_flag_t    flags,
                 rocprofiler_counter_record_t* output_records, size_t* record_count));

    MOCK_METHOD(rocprofiler_status_t, create_context,
                (rocprofiler_context_id_t * context));

    MOCK_METHOD(rocprofiler_status_t, create_buffer,
                (rocprofiler_context_id_t context, size_t size, size_t watermark,
                 rocprofiler_buffer_policy_t     policy,
                 rocprofiler_buffer_tracing_cb_t callback, void* callback_data,
                 rocprofiler_buffer_id_t* buffer_id));

    MOCK_METHOD(rocprofiler_status_t, configure_device_counting_service,
                (rocprofiler_context_id_t context, rocprofiler_buffer_id_t buffer,
                 rocprofiler_agent_id_t                   agent,
                 rocprofiler_device_counting_service_cb_t callback, void* user_data));

    MOCK_METHOD(rocprofiler_status_t, start_context, (rocprofiler_context_id_t context));

    MOCK_METHOD(rocprofiler_status_t, stop_context, (rocprofiler_context_id_t context));

    MOCK_METHOD(rocprofiler_status_t, create_profile_config,
                (rocprofiler_agent_id_t agent, rocprofiler_counter_id_t* counters,
                 size_t num_counters, rocprofiler_profile_config_id_t* profile));

    MOCK_METHOD(rocprofiler_status_t, iterate_agent_supported_counters,
                (rocprofiler_agent_id_t              agent,
                 rocprofiler_available_counters_cb_t callback, void* user_data));

    MOCK_METHOD(rocprofiler_status_t, query_counter_info,
                (rocprofiler_counter_id_t              counter,
                 rocprofiler_counter_info_version_id_t version, void* info));

    MOCK_METHOD(rocprofiler_status_t, query_record_counter_id,
                (rocprofiler_counter_instance_id_t record_id,
                 rocprofiler_counter_id_t*         counter_id));

    MOCK_METHOD(rocprofiler_status_t, query_record_dimension_position,
                (rocprofiler_counter_instance_id_t  record_id,
                 rocprofiler_counter_dimension_id_t dim_id, size_t* position));

    MOCK_METHOD(rocprofiler_status_t, iterate_counter_dimensions,
                (rocprofiler_counter_id_t              counter,
                 rocprofiler_available_dimensions_cb_t callback, void* user_data));

    MOCK_METHOD(rocprofiler_status_t, query_available_agents,
                (rocprofiler_agent_version_t             version,
                 rocprofiler_query_available_agents_cb_t callback, size_t agent_size,
                 void* user_data));
};

/**
 * @brief Factory for creating mock driver instances in tests.
 */
struct mock_driver_factory
{
    using driver_t = mock_driver;

    static std::shared_ptr<driver_t> create_driver()
    {
        return std::make_shared<driver_t>();
    }
};

}  // namespace rocprofsys::pmc::drivers::rocprofiler_sdk::testing

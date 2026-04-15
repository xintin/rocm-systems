// Copyright (c) Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT

#pragma once

#include "library/pmc/collectors/base/collector.hpp"
#include "library/pmc/collectors/sdk_pmc/sdk_pmc_traits.hpp"

namespace rocprofsys::pmc::collectors::sdk_pmc
{

/**
 * @brief SDK PMC metrics collector for GPU hardware performance counters.
 *
 * This collector specializes the base::collector template for rocprofiler-sdk
 * device_counting_service. All SDK PMC-specific behavior is defined in
 * sdk_pmc_traits.
 *
 * @tparam DeviceProvider Type providing device enumeration from the SDK bridge.
 * @tparam Config Configuration policy providing settings and output policies.
 */
template <typename DeviceProvider, typename Config>
using collector = base::collector<sdk_pmc_traits<DeviceProvider>, DeviceProvider, Config>;

}  // namespace rocprofsys::pmc::collectors::sdk_pmc

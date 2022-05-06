#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "VehicleHalClient.h"
#include "utils/Runnable.h"

namespace vhal_v2_0 = android::hardware::automotive::vehicle::V2_0;

namespace vendor {
namespace nlab {

using namespace utils;

namespace vehicle {
namespace V1_0 {

namespace impl {

using SyncedVehiclePropValue = std::pair<vhal_v2_0::VehiclePropValue, std::mutex>;

class Simulator
    : public VehicleHalClient
    , public Runnable
{
public:
    Simulator();
    std::vector<vhal_v2_0::VehiclePropConfig> getAllPropertyConfig() const override;

    vhal_v2_0::StatusCode setProperty(const vhal_v2_0::VehiclePropValue& value, bool updateStatus) override;

private:
    void routine(const std::atomic_bool& request_to_stop);
    bool isSupportedProperty(const vhal_v2_0::VehiclePropValue& value) const;

    vhal_v2_0::StatusCode setIfSupportedProperty(const vhal_v2_0::VehiclePropValue& value, bool updateStatus);
    vhal_v2_0::StatusCode setVendorTest1sCounter(const vhal_v2_0::VehiclePropValue& value, bool updateStatus);
    vhal_v2_0::StatusCode setVendorTest500msCounter(
        const vhal_v2_0::VehiclePropValue& value,
        bool updateStatus);
    vhal_v2_0::StatusCode setVendorTestSysProp(const vhal_v2_0::VehiclePropValue& value, bool updateStatus);

    void simulateVendorTest1sCounter();
    void simulateVendorTest500msCounter();
    void simulateVendorTestSysProp();

    std::thread mThread;

    SyncedVehiclePropValue mVendorTest1sCounter;
    SyncedVehiclePropValue mVendorTest500msCounter;
    SyncedVehiclePropValue mVendorTestSysProp;
};

} // namespace impl

} // namespace V1_0
} // namespace vehicle
} // namespace nlab
} // namespace vendor
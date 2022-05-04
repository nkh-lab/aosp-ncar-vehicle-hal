// Based on hardware/interfaces/automotive/vehicle/2.0/default/impl/vhal_v2_0/VehicleHalClient.h

#pragma once

#include <vhal_v2_0/VehicleClient.h>

namespace vhal_v2_0 = android::hardware::automotive::vehicle::V2_0;

namespace vendor {
namespace nlab {
namespace vehicle {
namespace V1_0 {

namespace impl {

// The common client operations that may be used by both native and
// virtualized VHAL clients.
class VehicleHalClient : public vhal_v2_0::IVehicleClient
{
public:
    // Type of callback function for handling the new property values
    using PropertyCallBackType =
        std::function<void(const vhal_v2_0::VehiclePropValue&, bool updateStatus)>;

    // Method from IVehicleClient
    void onPropertyValue(const vhal_v2_0::VehiclePropValue& value, bool updateStatus) override;

    void registerPropertyValueCallback(PropertyCallBackType&& callback);

private:
    PropertyCallBackType mPropCallback;
};

} // namespace impl

} // namespace V1_0
} // namespace vehicle
} // namespace nlab
} // namespace vendor

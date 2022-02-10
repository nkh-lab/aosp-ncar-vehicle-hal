#pragma once

#include <vhal_v2_0/VehicleHal.h>
#include <vhal_v2_0/VehiclePropertyStore.h>

namespace vhal_v2_0 = android::hardware::automotive::vehicle::V2_0;

namespace vendor {
namespace nlab {
namespace vehicle {
namespace V1_0 {

namespace impl {

class VehicleHalImpl : public vhal_v2_0::VehicleHal
{
public:
    VehicleHalImpl(vhal_v2_0::VehiclePropertyStore* propStore);
    ~VehicleHalImpl() = default;

    void onCreate() override;
    std::vector<vhal_v2_0::VehiclePropConfig> listProperties() override;
    VehiclePropValuePtr get(
        const vhal_v2_0::VehiclePropValue& requestedPropValue,
        vhal_v2_0::StatusCode* outStatus) override;
    vhal_v2_0::StatusCode set(const vhal_v2_0::VehiclePropValue& propValue) override;
    vhal_v2_0::StatusCode subscribe(int32_t property, float sampleRate) override;
    vhal_v2_0::StatusCode unsubscribe(int32_t property) override;

private:
    void regPropsInPropStore();
    void initPropsInPropStore();

    vhal_v2_0::VehiclePropertyStore* mPropStore;
};

} // namespace impl

} // namespace V1_0
} // namespace vehicle
} // namespace nlab
} // namespace vendor
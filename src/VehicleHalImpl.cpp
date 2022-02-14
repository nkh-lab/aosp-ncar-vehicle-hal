#define LOG_TAG "VehicleHalImpl.cpp"
#include <log/log_main.h>

#include "VehicleHalImpl.h"

#include <vhal_v2_0/VehicleUtils.h>

#include "DefaultConfig.h"

namespace vendor {
namespace nlab {
namespace vehicle {
namespace V1_0 {

namespace impl {

VehicleHalImpl::VehicleHalImpl(vhal_v2_0::VehiclePropertyStore* propStore)
    : mPropStore(propStore)
{
    regPropsInPropStore();
}

void VehicleHalImpl::onCreate()
{
    initPropsInPropStore();
}

std::vector<vhal_v2_0::VehiclePropConfig> VehicleHalImpl::listProperties()
{
    return mPropStore->getAllConfigs();
}

VehicleHalImpl::VehiclePropValuePtr VehicleHalImpl::get(
    const vhal_v2_0::VehiclePropValue& requestedPropValue,
    vhal_v2_0::StatusCode* outStatus)
{
    VehiclePropValuePtr v = nullptr;

    auto internalPropValue = mPropStore->readValueOrNull(requestedPropValue);

    if (internalPropValue != nullptr)
    {
        v = getValuePool()->obtain(*internalPropValue);
    }

    *outStatus = (v != nullptr ? vhal_v2_0::StatusCode::OK : vhal_v2_0::StatusCode::INVALID_ARG);

    return v;
}

vhal_v2_0::StatusCode VehicleHalImpl::set(const vhal_v2_0::VehiclePropValue& propValue)
{
    vhal_v2_0::StatusCode ret = vhal_v2_0::StatusCode::OK;

    if (!mPropStore->writeValue(propValue, false)) ret = vhal_v2_0::StatusCode::NOT_AVAILABLE;

    return ret;
}

vhal_v2_0::StatusCode VehicleHalImpl::subscribe(int32_t property, float sampleRate)
{
    static_cast<void>(property);
    static_cast<void>(sampleRate);
    return vhal_v2_0::StatusCode::OK;
}

vhal_v2_0::StatusCode VehicleHalImpl::unsubscribe(int32_t property)
{
    static_cast<void>(property);
    return vhal_v2_0::StatusCode::OK;
}

void VehicleHalImpl::regPropsInPropStore()
{
    for (const auto& it : kVehicleProperties) mPropStore->registerProperty(it.config);
}

void VehicleHalImpl::initPropsInPropStore()
{
    static constexpr bool shouldUpdateStatus = true;

    for (const auto& p : kVehicleProperties)
    {
        vhal_v2_0::VehiclePropValue prop = {.prop = p.config.prop};

        // a global property will have only a single area
        if (vhal_v2_0::isGlobalProp(prop.prop))
        {
            prop.areaId = 0;
            prop.value = p.initialValue;

            mPropStore->writeValue(prop, shouldUpdateStatus);
        }
        else
        {
            // create a separate instance for each individual zone
            for (auto const& ac : p.config.areaConfigs)
            {
                prop.areaId = ac.areaId;
                prop.value = p.initialValue; // use general if we can't find or if only one area

                auto search = p.initialAreaValues.find(ac.areaId);
                if (search != p.initialAreaValues.end()) prop.value = search->second;

                mPropStore->writeValue(prop, shouldUpdateStatus);
            }
        }
    }
}

} // namespace impl

} // namespace V1_0
} // namespace vehicle
} // namespace nlab
} // namespace vendor
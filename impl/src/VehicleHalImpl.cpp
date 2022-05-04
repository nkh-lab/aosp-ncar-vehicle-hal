#define LOG_TAG __FILE_NAME__
#include <log/log_main.h>

#include "VehicleHalImpl.h"

#include <utils/SystemClock.h>

#include <vhal_v2_0/VehicleUtils.h>

#include "DefaultConfig.h"

namespace {

constexpr std::chrono::nanoseconds hertzToNanoseconds(float hz)
{
    return std::chrono::nanoseconds(static_cast<int64_t>(1000000000L / hz));
}

} // namespace

namespace vendor {
namespace nlab {
namespace vehicle {
namespace V1_0 {

namespace impl {

VehicleHalImpl::VehicleHalImpl(vhal_v2_0::VehiclePropertyStore* propStore, VehicleHalClient* client)
    : mPropStore(propStore)
    , mVehicleClient(client)
    , mRecurrentTimer(
          std::bind(&VehicleHalImpl::onContinuousPropertyTimer, this, std::placeholders::_1))
{
    regPropsInPropStore();

    mVehicleClient->registerPropertyValueCallback(std::bind(
        &VehicleHalImpl::onPropertyValue, this, std::placeholders::_1, std::placeholders::_2));
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

    auto currentPropValue = mPropStore->readValueOrNull(propValue);

    if (currentPropValue == nullptr)
    {
        ret = StatusCode::INVALID_ARG;
    }
    else if (currentPropValue->status != VehiclePropertyStatus::AVAILABLE)
    {
        return StatusCode::NOT_AVAILABLE;
    }
    else
    {
        ret = mVehicleClient->setProperty(propValue, false);
        if (ret == vhal_v2_0::StatusCode::OK) mPropStore->writeValue(propValue, false);
    }

    return ret;
}

vhal_v2_0::StatusCode VehicleHalImpl::subscribe(int32_t property, float sampleRate)
{
    if (isContinuousProperty(property))
    {
        ALOGD("%s(): ContinuousProperty: %d, sampleRate: %f", __func__, property, sampleRate);
        mRecurrentTimer.registerRecurrentEvent(hertzToNanoseconds(sampleRate), property);
    }
    return StatusCode::OK;
}

vhal_v2_0::StatusCode VehicleHalImpl::unsubscribe(int32_t property)
{
    if (isContinuousProperty(property))
    {
        mRecurrentTimer.unregisterRecurrentEvent(property);
    }
    return StatusCode::OK;
}

void VehicleHalImpl::onPropertyValue(const vhal_v2_0::VehiclePropValue& value, bool updateStatus)
{
    vhal_v2_0::VehicleHal::VehiclePropValuePtr updatedPropValue =
        vhal_v2_0::VehicleHal::getValuePool()->obtain(value);

    if (mPropStore->writeValue(*updatedPropValue, updateStatus))
    {
        vhal_v2_0::VehicleHal::doHalEvent(std::move(updatedPropValue));
    }
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

void VehicleHalImpl::onContinuousPropertyTimer(const std::vector<int32_t>& properties)
{
    VehiclePropValuePtr v;

    auto& pool = *getValuePool();

    for (int32_t property : properties)
    {
        if (isContinuousProperty(property))
        {
            auto internalPropValue = mPropStore->readValueOrNull(property);
            if (internalPropValue != nullptr)
            {
                v = pool.obtain(*internalPropValue);
            }
        }
        else
        {
            ALOGE("%s(): Unexpected for property: %d", __func__, property);
        }

        if (v.get())
        {
            v->timestamp = android::elapsedRealtimeNano();
            doHalEvent(std::move(v));
        }
    }
}

bool VehicleHalImpl::isContinuousProperty(int32_t propId) const
{
    const VehiclePropConfig* config = mPropStore->getConfigOrNull(propId);
    if (config == nullptr)
    {
        ALOGW("%s(): Config not found for property: %d", __func__, propId);
        return false;
    }
    return config->changeMode == VehiclePropertyChangeMode::CONTINUOUS;
}

} // namespace impl

} // namespace V1_0
} // namespace vehicle
} // namespace nlab
} // namespace vendor

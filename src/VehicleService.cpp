#define LOG_TAG "VehicleService.cpp"
#include <log/log_main.h>

#include <chrono>
#include <thread>

#include <hidl/HidlTransportSupport.h>
#include <vhal_v2_0/VehicleHalManager.h>

#include "VehicleHalImpl.h"

using namespace std::chrono_literals;

using namespace vendor::nlab::vehicle::V1_0;

using namespace android;
using namespace android::hardware;
using namespace android::hardware::automotive::vehicle::V2_0;

int main(int argc, char const* argv[])
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    ALOGI("Hello World!\n");

    auto store = std::make_unique<vhal_v2_0::VehiclePropertyStore>();
    auto hal = std::make_unique<impl::VehicleHalImpl>(store.get());
    auto service = std::make_unique<VehicleHalManager>(hal.get());

    configureRpcThreadpool(1, true); // TODO: to understand

    ALOGI("Registering as service...");
    status_t status = service->registerAsService();

    if (status != OK)
    {
        ALOGE("Unable to register vehicle service (%d)", status);
        return 1;
    }

    ALOGI("Vehicle service ready");
    joinRpcThreadpool();

    return 0;
}
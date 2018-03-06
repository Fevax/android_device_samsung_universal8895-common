#include <stdio.h>
#include <stdlib.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "log.h"
#include "property_service.h"
#include "util.h"
#include "vendor_init.h"

#include "init_sec.h"

#define MODEL_NAME_LEN 5  // e.g. "G955F"
#define BUILD_NAME_LEN 8  // e.g. "XXU1CRAP"
#define CODENAME_LEN   11 // e.g. "dream2ltexx"


static void property_override(char const prop[], char const value[]) {
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void vendor_load_properties()
{
    const std::string bootloader = property_get("ro.bootloader");
    const std::string bl_model = bootloader.substr(0, MODEL_NAME_LEN);
    const std::string bl_build = bootloader.substr(MODEL_NAME_LEN);

    std::string model;  // G955F
    std::string device; // dream2lte
    std::string name;    // dream2ltexx
    std::string description;
    std::string fingerprint;

    model = "SM-" + bl_model;

    for (size_t i = 0; i < VARIANT_MAX; i++) {
        std::string model_ = all_variants[i]->model;
        if (model.compare(model_) == 0) {
            device = all_variants[i]->codename;
            break;
        }
    }

    if (device.size() == 0) {
        ERROR("Could not detect device, forcing dream2lte");
        device = "dream2lte";
    }

    name = device + "xx";

    description = name + "-user 8.0.0 R16NW " + bl_model + bl_build + " release-keys";
    fingerprint = "samsung/" + name + "/" + device + ":8.0.0/R16NW/" + bl_model + bl_build + ":user/release-keys";

    INFO("Found bootloader: %s", bootloader.c_str());
    INFO("Setting ro.product.model: %s", model.c_str());
    INFO("Setting ro.product.device: %s", device.c_str());
    INFO("Setting ro.product.name: %s", name.c_str());
    INFO("Setting ro.build.product: %s", device.c_str());
    INFO("Setting ro.build.description: %s", description.c_str());
    INFO("Setting ro.build.fingerprint: %s", fingerprint.c_str());

    property_override("ro.product.model", model.c_str());
    property_override("ro.product.device", device.c_str());
    property_override("ro.product.name", name.c_str());
    property_override("ro.build.product", device.c_str());
    property_override("ro.build.description", description.c_str());
    property_override("ro.build.fingerprint", fingerprint.c_str());
}

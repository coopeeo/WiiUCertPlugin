#pragma once
#include <wups.h>
#include <wups/config.h>
#include <wups/config/WUPSConfigCategory.h>
#include <wups/config/WUPSConfigItem.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemIntegerRange.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <wups/config/WUPSConfigItemStub.h>

#define PATCH_CERTS_CONFIG_ID "patch_certs"
#define PATCH_CERTS_DEFUALT_VALUE true


namespace config {

    extern bool patchCerts;

    void patchCertsChanged(ConfigItemBoolean *item, bool newValue);

    WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle);
    
    void ConfigMenuClosedCallback();
    void InitializeConfig();
} // namespace config

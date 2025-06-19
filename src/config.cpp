#include <wups.h>
#include <wups/config.h>
#include <wups/config/WUPSConfigCategory.h>
#include <wups/config/WUPSConfigItem.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemIntegerRange.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <wups/config/WUPSConfigItemStub.h>
#include <wups/storage.h>
#include <coreinit/title.h>
#include <sysapp/launch.h>

#include "config.hpp"
#include "utils/logger.h"

namespace config {

  bool patchCerts = PATCH_CERTS_DEFUALT_VALUE;
  bool needRelaunch = false;

  // Patch Certificates setting event
  void patchCertsChanged(ConfigItemBoolean *item, bool newValue) {
    if (newValue != patchCerts) {
      WUPSStorageAPI::Store(PATCH_CERTS_CONFIG_ID, newValue);
    }

    patchCerts = newValue;
  }

  // Open event for the Aroma config menu
  WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle) {
    WUPSConfigCategory root = WUPSConfigCategory(rootHandle);

    try {
      // Add setting items
      root.add(WUPSConfigItemStub::Create("-- Main --"));
      root.add(WUPSConfigItemBoolean::Create(PATCH_CERTS_CONFIG_ID, "Patch Certificates", PATCH_CERTS_DEFUALT_VALUE, patchCerts, patchCertsChanged));
      
    } catch (std::exception &e) {
      DEBUG_FUNCTION_LINE("Creating config menu failed: %s", e.what());
      return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
    }

    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
  }

  // Close event for the Aroma config menu
  void ConfigMenuClosedCallback() {
    WUPSStorageAPI::SaveStorage();
    if (needRelaunch) {
      _SYSLaunchTitleWithStdArgsInNoSplash(OSGetTitleID(), nullptr);
      needRelaunch = false;
    }
  }

  // Config stuff in plugin initialization
  void InitializeConfig() {
      // Add the config
    WUPSConfigAPIOptionsV1 configOptions = {.name = "Root Certificate Patcher"};
    if (WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback,
                          ConfigMenuClosedCallback) !=
        WUPSCONFIG_API_RESULT_SUCCESS) {
      DEBUG_FUNCTION_LINE("Failed to init config api");
    }

    // Add get saved values
    WUPSStorageError storageRes;
    if ((storageRes = WUPSStorageAPI::GetOrStoreDefault(PATCH_CERTS_CONFIG_ID, patchCerts, PATCH_CERTS_DEFUALT_VALUE)) != WUPS_STORAGE_ERROR_SUCCESS) {
      DEBUG_FUNCTION_LINE("GetOrStoreDefault failed: %s (%d)", WUPSStorageAPI_GetStatusStr(storageRes), storageRes);
    }
  }

} // namespace config
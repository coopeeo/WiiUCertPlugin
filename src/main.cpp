#include <wups.h>
#include <coreinit/title.h>
#include <function_patcher/function_patching.h>
#include <notifications/notifications.h>

#include "config.hpp"

#include "utils/Notification.hpp"
#include "utils/logger.h"

WUPS_PLUGIN_NAME("Wii U Root Certificates");
WUPS_PLUGIN_DESCRIPTION("Adds newer root certificates to the Wii U's Internet Browser");
WUPS_PLUGIN_VERSION("v1.0.0");
WUPS_PLUGIN_AUTHOR("Cooper");
WUPS_PLUGIN_LICENSE("GPLv2");

WUPS_USE_STORAGE("certplugin");
WUPS_USE_WUT_DEVOPTAB();

INITIALIZE_PLUGIN() {
  // Initialize libraries
  WHBLogModuleInit();
  WHBLogUdpInit();
  WHBLogCafeInit();
  FunctionPatcher_InitLibrary();

  config::InitializeConfig();

  // Check if NotificationModule library is initialized
  if (NotificationModule_InitLibrary() != NOTIFICATION_MODULE_RESULT_SUCCESS) {
    DEBUG_FUNCTION_LINE("NotificationModule_InitLibrary failed");
  }

  if (config::patchCerts) {
    ShowNotification("Browser Patch enabled");
  } else {
    ShowNotification("Browser Patch disabled");
  }
}

DEINITIALIZE_PLUGIN() {
  WHBLogModuleDeinit();
  WHBLogUdpDeinit();
  WHBLogCafeDeinit();
  NotificationModule_DeInitLibrary();
  FunctionPatcher_DeInitLibrary();
}

ON_APPLICATION_START() {
  WHBLogModuleInit();
  WHBLogUdpInit();
  WHBLogCafeInit();
}

ON_APPLICATION_ENDS() {
  WHBLogModuleDeinit();
  WHBLogUdpDeinit();
  WHBLogCafeDeinit();
}
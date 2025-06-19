#include <wups.h>
#include <nsysnet/nssl.h>
#include <coreinit/filesystem.h>

#include "../utils/Notification.hpp"
#include "../utils/logger.h"
#include "../config.hpp"

#include "rootca_pem.h" // Included at runtime

static std::optional<FSFileHandle> rootca_handle{};

#define SURF_ROOTCA_PATH "vol/content/browser/rootca.pem"

DECL_FUNCTION(FSStatus, FSOpenFile_SURF, FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode, FSFileHandle *handle,
              FSErrorFlag errorMask) {

    DEBUG("Wii U wants to open file: %s", path);

    if (config::patchCerts && strcmp(SURF_ROOTCA_PATH, path) == 0) {
        FSStatus res = real_FSOpenFile_SURF(client, block, path, mode, handle, errorMask);
        rootca_handle = *handle;
        return res;
    }

    return real_FSOpenFile_SURF(client, block, path, mode, handle, errorMask);
}

DECL_FUNCTION(FSStatus, FSReadFile_SURF, FSClient *client, FSCmdBlock *block, uint8_t *buffer, uint32_t size, uint32_t count,
              FSFileHandle handle, uint32_t unk1, uint32_t flags) {
    if (size != 1) {
        DEBUG_FUNCTION_LINE("Falied to patch rootca pem. Size is not 1");
    }

    if (rootca_handle && *rootca_handle == handle) {
        DEBUG_FUNCTION_LINE("Trying to read root ca detected, returning patched data.");
        strlcpy((char *) buffer, (const char *) rootca_pem, size * count);
        return (FSStatus) count;
    }

    return real_FSReadFile_SURF(client, block, buffer, size, count, handle, unk1, flags);
}

DECL_FUNCTION(FSStatus, FSCloseFile_SURF, FSClient *client, FSCmdBlock *block, FSFileHandle handle, FSErrorFlag errorMask) {
    if (handle == rootca_handle) {
        DEBUG("Closing root ca file and resetting handle");
        rootca_handle.reset();
    }

    return real_FSCloseFile_SURF(client, block, handle, errorMask);
}

DECL_FUNCTION(NSSLError, NSSLInit_SURF) {
    WHBLogModuleInit();
    WHBLogUdpInit();
    WHBLogCafeInit();
    DEBUG_FUNCTION_LINE("NSSLInit called, launched debug logging");
    return real_NSSLInit_SURF();
}

WUPS_MUST_REPLACE_FOR_PROCESS(NSSLInit_SURF, WUPS_LOADER_LIBRARY_NSYSNET, NSSLInit, WUPS_FP_TARGET_PROCESS_BROWSER);
WUPS_MUST_REPLACE_FOR_PROCESS(FSOpenFile_SURF, WUPS_LOADER_LIBRARY_COREINIT, FSOpenFile, WUPS_FP_TARGET_PROCESS_BROWSER);
WUPS_MUST_REPLACE_FOR_PROCESS(FSReadFile_SURF, WUPS_LOADER_LIBRARY_COREINIT, FSReadFile, WUPS_FP_TARGET_PROCESS_BROWSER);
WUPS_MUST_REPLACE_FOR_PROCESS(FSCloseFile_SURF, WUPS_LOADER_LIBRARY_COREINIT, FSCloseFile, WUPS_FP_TARGET_PROCESS_BROWSER);
#pragma once
#include "auth/auth.hpp"
#include "auth/skStr.h"
#include <mutex>

// KeyAuth: "Application not found. Please copy strings directly from dashboard." means these
// values do NOT match your app on https://keyauth.cc (or your seller panel) exactly.
// Fix: open Application Settings on the dashboard and copy:
//   - Application Name  -> must match `name` below (character-for-character, case-sensitive)
//   - Owner ID          -> must match `ownerid`
//   - Application Version -> must match `version` (often "1.0")
// UI branding (e.g. Blaze Xiters) is separate; this `name` is only the KeyAuth app identifier.
namespace KeyAuthClient {
    using namespace KeyAuth;

    const std::string name = skCrypt("VOID XITERS INTERNAL").decrypt();
    const std::string ownerid = skCrypt("tnktGBPFCy").decrypt();
    const std::string version = skCrypt("1.0").decrypt();
    const std::string url = skCrypt("https://keyauth.win/api/1.3/").decrypt();
    const std::string path = skCrypt("").decrypt(); // (OPTIONAL) see tutorial here https://www.youtube.com/watch?v=I9rxt821gMk&t=1s

    api Internal(name, ownerid, version, url, path);

    std::once_flag init_once_flag;
    inline void EnsureInit() {
        std::call_once(init_once_flag, []() {
            Internal.init();
            });
    }
}

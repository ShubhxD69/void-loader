#pragma once

// Build: place the payload DLL at src/BLAZE FREE INTERNAL.dll (see resources.rc RCDATA 101).
// Runtime: the loader writes a copy under %TEMP% with a random name only when you inject — not beside the .exe.

// KeyAuth application credentials: edit Keyauth.h (name, ownerid, version, url).

// Debug Configuration
#define DEBUG_MODE false

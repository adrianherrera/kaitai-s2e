///
/// Copyright (C) 2017, Adrian Herrera
/// All rights reserved.
///
/// Copy this file to source/s2e/guest/common/include/kaitai in your S2E
/// environment.
///

#ifndef S2E_KAITAI_COMMANDS_H
#define S2E_KAITAI_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

// Indicates which command is being sent to S2E
enum S2E_KAITAI_COMMANDS {
    KAITAI_MAKE_SYMBOLIC,
};

//
// The command to selectively make the data at `InputFile` symbolic
//
// Note: We could have added more fields to this command, e.g. the name/path of
// the input file. This would have allowed us to specify which *.ksy file to
// apply to which input file (e.g. in s2e-config.lua).
//
struct S2E_KAITAI_COMMAND_MAKE_SYMBOLIC {
    // Pointer to guest memory where the symbolic file has been loaded
    uint64_t InputFile;

    // Size of the input file (in bytes)
    uint64_t FileSize;

    // 1 on success, 0 on failure
    uint64_t Result;
} __attribute__((packed));

// The command sent to S2E's `KaitaiInterface` plugin
struct S2E_KAITAI_COMMAND {
    enum S2E_KAITAI_COMMANDS Command;
    union {
        struct S2E_KAITAI_COMMAND_MAKE_SYMBOLIC MakeSymbolic;
    };
} __attribute__((packed));

#ifdef __cplusplus
}
#endif

#endif

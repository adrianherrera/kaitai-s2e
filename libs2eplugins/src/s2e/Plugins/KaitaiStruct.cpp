///
/// Copyright (C) 2017, Adrian Herrera
/// All rights reserved.
///
/// Copy this file to source/s2e/libs2eplugins/src/s2e/Plugins in you S2E
/// environment.
///

// From source/s2e/guest/common/include
#include <s2e/kaitai/commands.h>

#include <s2e/S2E.h>
#include <s2e/Utils.h>

#include <s2e/Plugins/Lua/Lua.h>
#include <s2e/Plugins/Lua/LuaS2EExecutionState.h>

#include "KaitaiStruct.h"

namespace s2e {
namespace plugins {

S2E_DEFINE_PLUGIN(KaitaiStruct, "Combine S2E and Kaitai Struct", "", "BaseInstructions", "LuaBindings");

void KaitaiStruct::initialize() {
    m_kaitaiParserFunc = s2e()->getConfig()->getString(getConfigKey() + ".parser");
}

bool KaitaiStruct::handleMakeSymbolic(S2EExecutionState *state, const S2E_KAITAI_COMMAND &command) {
    uint64_t addr = command.MakeSymbolic.InputFile;
    uint64_t size = command.MakeSymbolic.FileSize;
    std::vector<uint8_t> data(size);

    // Read the input file's contents from guest memory
    if (!state->mem()->readMemoryConcrete(addr, data.data(), sizeof(uint8_t) * size)) {
        return false;
    }

    // Get the Lua interpreter state
    lua_State *L = s2e()->getConfig()->getState();

    // Wrap the current S2E execution state so that it can be passed to Lua code
    LuaS2EExecutionState luaS2EState(state);

    // Turn the input file into a Lua string
    luaL_Buffer luaBuff;
    luaL_buffinit(L, &luaBuff);
    luaL_addlstring(&luaBuff, (char*) data.data(), sizeof(uint8_t) * size);

    // Push the arguments to our Kaitai Struct parser function onto the (Lua)
    // stack.
    //
    // The execution state is pushed first, followed by the start address of
    // input file (in guest memory), followed by the contents of the input file
    lua_getglobal(L, m_kaitaiParserFunc.c_str());
    Lunar<LuaS2EExecutionState>::push(L, &luaS2EState);
    lua_pushinteger(L, addr);
    luaL_pushresult(&luaBuff);

    // Call our Kaitai Struct parser function
    lua_call(L, 3, 0);

    // We should probably do some error handling here...

    return true;
}

void KaitaiStruct::handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize) {
    S2E_KAITAI_COMMAND cmd;

    // Validate the recevied command
    if (guestDataSize != sizeof(cmd)) {
        getWarningsStream(state) << "S2E_KAITAI_COMMAND: Mismatched command structure size " << guestDataSize << "\n";
        exit(1);
    }

    // Read the command
    if (!state->mem()->readMemoryConcrete(guestDataPtr, &cmd, guestDataSize)) {
        getWarningsStream(state) << "S2E_KAITAI_COMMAND: Failed to read command\n";
        exit(1);
    }

    // Handle the command
    switch (cmd.Command) {
        case KAITAI_MAKE_SYMBOLIC: {
            bool success = handleMakeSymbolic(state, cmd);
            cmd.MakeSymbolic.Result = success ? 0 : 1;

            // Write the result back to the guest
            if (!state->mem()->writeMemory(guestDataPtr, cmd)) {
                getWarningsStream(state) << "S2E_KAITAI_COMMAND: Failed to write result to guest\n";
                exit(1);
            }
        } break;

        default: {
            getWarningsStream(state) << "S2E_KAITAI_COMMAND: Invalid command " << hexval(cmd.Command) << "\n";
            exit(1);
        }
    }
}

} // namespace plugins
} // namespace s2e

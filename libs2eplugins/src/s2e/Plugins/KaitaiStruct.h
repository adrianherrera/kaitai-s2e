///
/// Copyright (C) 2017, Adrian Herrera
/// All rights reserved.
///
/// Copy this file to source/s2e/libs2eplugins/src/s2e/Plugins in your S2E
/// environment.
///

#ifndef S2E_PLUGINS_KAITAI_STRUCT_H
#define S2E_PLUGINS_KAITAI_STRUCT_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/Core/BaseInstructions.h>

struct S2E_KAITAI_COMMAND;

namespace s2e {
namespace plugins {

class KaitaiStruct : public Plugin, public IPluginInvoker {
    S2E_PLUGIN

public:
    KaitaiStruct(S2E *s2e) : Plugin(s2e) {
    }

    void initialize();

    /**
     * The method from IPluginInvoker that we must implement to respond to a
     * custom command. This method takes the current S2E state, a pointer to
     * the custom command object (in guest memory) and the size of the custom
     * command object.
     */
    virtual void handleOpcodeInvocation(S2EExecutionState *state, uint64_t guestDataPtr, uint64_t guestDataSize);

private:
    /// The name of the Lua function that will run the Kaitai Struct parser
    std::string m_kaitaiParserFunc;

    /// handleOpcodeInvocation will call this method to actually invoke the Lua function
    bool handleMakeSymbolic(S2EExecutionState *state, const S2E_KAITAI_COMMAND &command);
};

} // namespace plugins
} // namespace s2e

#endif

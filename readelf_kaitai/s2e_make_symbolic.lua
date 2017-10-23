--
-- Copyright (C) 2017, Adrian Herrera
-- All rights reserved.
--
-- Copy this file to Kaitai Struct's Lua runtime directory.
--

local class = require("class")

S2eMakeSymbolic = class.class()

--
-- The S2eMakeSymbolic class constructor.
--
-- Paramters:
--   state: A wrapped S2EExecutionState object
--   start_addr: The start address of the input file (in guest memory)
--   curr_pos: The current position of the parser
--   name: The name to assign the symbolic variable
--
function S2eMakeSymbolic:_init(state, start_addr, curr_pos, name)
    self._state = state
    self._addr = start_addr + curr_pos
    self._name = name
end

--
-- This function is traditionally used by the processing spec to transform the
-- input data in some way (e.g. decrypt, decompress, etc.). However, we will
-- only use this function to make the memory region symbolic. We will return
-- the data unchanged.
--
function S2eMakeSymbolic:decode(data)
    local mem = self._state:mem()
    local size = data:len()

    -- The decode routine is called _after_ the data has already been read, so
    -- we must return to the start of the data in order to make it symbolic
    local addr = self._addr - size

    mem:makeConcolic(addr, size, self._name)

    -- Return the data unchanged
    return data
end

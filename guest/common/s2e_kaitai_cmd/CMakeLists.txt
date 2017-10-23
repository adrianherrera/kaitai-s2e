#
# Copyright (C) 2017, Adrian Herrera
# All rights reserved.
#
# Copy this file to source/s2e/guest/common/s2e_kaitai_cmd in your S2E
# environment.
#
# Don't forget to add the following line to
# source/s2e/guest/common/CMakeLists.txt:
#
#   add_subdirectory(s2e_kaitai_cmd)
#

add_executable(s2e_kaitai_cmd s2e_kaitai_cmd.c)

install(TARGETS s2e_kaitai_cmd RUNTIME DESTINATION .)

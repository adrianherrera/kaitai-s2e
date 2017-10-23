///
/// Copyright (C) 2017, Adrian Herrera
/// All rights reserved.
///
/// Copy this file to source/s2e/guest/common/s2e_kaitai_cmd in your S2E
/// environment.
///

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <s2e/s2e.h>
#include <s2e/kaitai/commands.h>

// Send the input file buffer to S2E to selectively make it symbolic
static inline int s2e_kaitai_make_symbolic(const uint8_t *buffer, unsigned size) {
    struct S2E_KAITAI_COMMAND cmd = {0};

    cmd.Command = KAITAI_MAKE_SYMBOLIC;
    cmd.MakeSymbolic.InputFile = (uintptr_t) buffer;
    cmd.MakeSymbolic.FileSize = size;
    cmd.MakeSymbolic.Result = 0;

    s2e_invoke_plugin("KaitaiStruct", &cmd, sizeof(cmd));

    return (int) cmd.MakeSymbolic.Result;
}

int main(int argc, char *argv[]) {
    int retval = 0;

    if (argc < 2) {
        fprintf(stderr, "USAGE: %s /path/to/input/file\n", argv[0]);
        return 1;
    }

    char *input_file_path = argv[1];
    int flags = O_RDWR;

#ifdef _WIN32
    flags |= O_BINARY;
#endif

    // Open the input file (to make symbolic) for reading
    int fd = open(input_file_path, flags);
    if (fd < 0) {
        s2e_kill_state_printf(1, "s2e_kaitai: could not open %s\n", input_file_path);
        return 1;
    }

    // Determine the size of the input file
    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        s2e_kill_state_printf(1, "s2e_kaitai: could not determine size of %s\n", input_file_path);
        retval = 1;
        goto input_file_close;
    }

    // Return to the start of the input file
    lseek(fd, 0, SEEK_SET);

    // Allocate a buffer for the input file
    uint8_t *input_buffer = malloc(sizeof(uint8_t) * size);
    if (!input_buffer) {
        s2e_kill_state_printf(1, "s2e_kaitai: could not allocate memory for %s\n", input_file_path);
        retval = 1;
        goto input_file_close;
    }

    // Read the input file
    if (read(fd, input_buffer, size) < 0) {
        s2e_kill_state_printf(1, "s2e_kaitai: could not read %s\n", input_file_path);
        retval = 1;
        goto input_buffer_free;
    }

    // Check that we read all of the input file
    off_t bytes_read = lseek(fd, 0, SEEK_CUR);
    if (bytes_read != size) {
        s2e_kill_state_printf(1, "s2e_kaitai: failed to read all of %s\n", input_file_path);
        retval = 1;
        goto input_buffer_free;
    }

    // Invoke the Kaitai Struct plugin to selectively make the buffer symbolic
    if (s2e_kaitai_make_symbolic(input_buffer, size)) {
        s2e_kill_state_printf(1, "s2e_kaitai: failed to make %s symbolic\n", input_file_path);
        retval = 1;
        goto input_buffer_free;
    }

    // Seek to the start of the file
    if (lseek(fd, 0, SEEK_SET) < 0) {
        s2e_kill_state_printf(1, "s2e_kaitai: failed to seek to start of %s\n", input_file_path);
        retval = 1;
        goto input_buffer_free;
    }

    // Write back the symbolic data
    if (write(fd, input_buffer, size) != size) {
        s2e_kill_state_printf(1, "s2e_kaitai: failed to write back to %s\n", input_file_path);
    }

    // Clean up
input_buffer_free:
    free(input_buffer);

input_file_close:
    close(fd);

    return retval;
}

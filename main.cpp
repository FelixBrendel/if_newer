/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Felix Brendel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#  include <Windows.h>
#else
#  include <sys/stat.h>
#  include <sys/types.h>
#endif

#define console_green    "\x1B[0;32m"
#define console_normal   "\x1B[0m"
#define console_magenta  "\x1B[0;35m"

auto file_newer(char* file_a, char* file_b) -> bool {
#ifdef _MSC_VER
    // windows land

    HANDLE   handle_a = 0;
    HANDLE   handle_b = 0;
    FILETIME last_write_a;
    FILETIME last_write_b;

    handle_a = CreateFileA(file_a, 0, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
    handle_b = CreateFileA(file_b, 0, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);

    if(handle_a == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "\n  ERROR: file %s was not found\n", file_a);
        exit(1);
    }

    if (handle_b == INVALID_HANDLE_VALUE)
        return true;

    GetFileTime(handle_a, NULL, NULL, &last_write_a);
    GetFileTime(handle_b, NULL, NULL, &last_write_b);

    return CompareFileTime(&last_write_a, &last_write_b) > 0;
#else
    // linux land
    struct stat attr_a;
    struct stat attr_b;
    if (stat(file_a, &attr_a) != 0) {
        fprintf(stderr, "\n  ERROR: file %s was not found\n", file_a);
        exit(1);
    }

    if (stat(file_b, &attr_b) != 0)
        return true;

    return attr_a.st_mtim.tv_sec > attr_b.st_mtim.tv_sec;
#endif
}

auto main(int argc, char** argv) -> int {
    if (argc < 4) {
        printf("Info:\n"
               "=====\n"
               "  Invokes a command with arguments if a file is newer than another\n"
               "\n"
               "Usage:\n"
               "======\n"
               "  if_newer <file A> <file B> <cmd> <args> ...\n");
        return 1;
    }

    int chars_printed = printf("%s -> %s", argv[1], argv[2]);
    int padding = 82-chars_printed;
    if (padding < 0)
        padding = 0;


    if (file_newer(argv[1], argv[2])) {
        char*  cmd = argv[3];
        char** cmd_args = &argv[4];
        int    cmd_args_count = argc - 4;

        // NOTE(Felix): one null terminator and a space after each arg (also
        // after the last one to keep the code simple)
        size_t cmd_len = 1 + cmd_args_count + strlen(cmd);

        for (int i = 0; i < cmd_args_count; ++i) {
            cmd_len += strlen(cmd_args[i]);
        }

        char* full_cmd_line = (char*)alloca(cmd_len);
        strcpy(full_cmd_line, cmd);

        for (int i = 0; i < cmd_args_count; ++i) {
            strcat(full_cmd_line, " ");
            strcat(full_cmd_line, cmd_args[i]);
        }

        printf(console_magenta "%*s[running]\n", padding, "");
        printf("  -> %s\n" console_normal, full_cmd_line);
        fflush(stdout);

        return system(full_cmd_line);
    } else {
        printf(console_green "%*s[skipped]\n" console_normal, padding, "");
    }

    return 0;
}

cmake_minimum_required(VERSION 3.26)
project(backlog_itmo_c_png_dandobri_main C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(.github/workflows/zlib)

add_executable(backlog_itmo_c_png_dandobri_main
        .github/workflows/zlib/adler32.c
        .github/workflows/zlib/compress.c
        .github/workflows/zlib/crc32.c
        .github/workflows/zlib/crc32.h
        .github/workflows/zlib/deflate.c
        .github/workflows/zlib/deflate.h
        .github/workflows/zlib/infback.c
        .github/workflows/zlib/inffast.c
        .github/workflows/zlib/inffast.h
        .github/workflows/zlib/inffixed.h
        .github/workflows/zlib/inflate.c
        .github/workflows/zlib/inflate.h
        .github/workflows/zlib/inftrees.c
        .github/workflows/zlib/inftrees.h
        .github/workflows/zlib/trees.c
        .github/workflows/zlib/trees.h
        .github/workflows/zlib/uncompr.c
        .github/workflows/zlib/zconf.h
        .github/workflows/zlib/zlib.h
        .github/workflows/zlib/zutil.c
        .github/workflows/zlib/zutil.h
        decode.c
        decode.h
        main.c
        png_file.c
        png_file.h
        return_codes.h
        write_to_file.c
        write_to_file.h)

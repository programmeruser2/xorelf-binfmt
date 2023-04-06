#!/bin/sh
binpath=XORELF_BIN_PATH
echo ":xorelf:M::XELF::$binpath/xorelf-interpreter:" > /proc/sys/fs/binfmt_misc/register

#!/bin/bash
# Build VERSION.dll proxy for VULN-001 DLL Hijacking PoC
# Run from WSL: cd /mnt/c/Users/Jose/Desktop/Proyectos/MuMuNxVbox-LPE/poc/dll_hijack && bash build.sh

set -e

echo "[*] Compiling VERSION.dll proxy (x86_64)..."
x86_64-w64-mingw32-gcc \
    -shared \
    -o VERSION.dll \
    version_hijack.c \
    version.def \
    -ladvapi32 \
    -Wall -Wextra \
    -O2 \
    -s

echo "[+] Built: VERSION.dll ($(stat -c%s VERSION.dll) bytes)"


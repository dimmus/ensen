#!/bin/bash

rm -rf build
meson setup . build -Db_coverage=true
cd build
meson compile
meson test
cd ../
ninja coverage -C build

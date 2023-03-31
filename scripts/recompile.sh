#!/bin/bash

source ./scripts/project_version_autoinc.sh
source ./scripts/remove.sh

meson setup . ./build
ninja -C ./build
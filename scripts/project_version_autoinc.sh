#!/bin/bash

# read the current version number from the meson.build file
CURRENT_VERSION=$(grep -m1 -o "version\s*:\s*['\"][^'\"]\+['\"]" meson.build | grep -o "[0-9]\+\.[0-9]\+\.[0-9]\+")

# echo "Current version: $CURRENT_VERSION"

# split the version number into its component parts
IFS='.' read -ra VERSION_PARTS <<< "$CURRENT_VERSION"

# increment the last part of the version number
((VERSION_PARTS[${#VERSION_PARTS[@]}-1]++))

# join the version parts back into a single string
NEW_VERSION=$(IFS='.'; echo "${VERSION_PARTS[*]}")

echo "New version: $NEW_VERSION"

# replace the old version string with the new one in the meson.build file
CURRENT_VERSION=$(echo $CURRENT_VERSION | sed 's/\//\\\//g')
NEW_VERSION=$(echo $NEW_VERSION | sed 's/\//\\\//g')
sed -i "s/$CURRENT_VERSION/$NEW_VERSION/" meson.build

# echo "Version updated!"


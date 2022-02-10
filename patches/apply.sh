#!/bin/bash

# Script should be linked to AOSP root folder and run from it

DIR=$(cd "$(dirname "$0")" && pwd)

PATCHES_DIR=$(dirname "$(readlink -f "$0")")/
PATCHES=$(find $PATCHES_DIR -name '*.patch')

for i in $PATCHES;
do
    PATCH_REL_PATH=$(dirname ${i#$PATCHES_DIR})
    cd $PATCH_REL_PATH
    git reset --hard --quiet
    git apply $i

    echo "Applying $(basename $i) to $PATCH_REL_PATH"

    cd $DIR
done

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

    if [ "$1" = "--reset" ]; then
        echo "Resetting changes in $(basename $i)"
    else
        git apply $i

        echo "Applying $(basename $i) to $PATCH_REL_PATH"
    fi

    cd $DIR
done

if [ "$1" != "--reset" ]; then
    echo "Attention!: For reverting applied patches re-run script with --reset parameter"
fi    

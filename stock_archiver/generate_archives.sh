#!/bin/bash

IMAGES_DIR="$1"
if [ -z "$IMAGES_DIR" ]; then
    IMAGES_DIR="./"
fi

ARCHIVES_DEFAULT="Archives"
ARCHIVE_DIR="$2"
if [ -z "$ARCHIVE_DIR" ]; then
    ARCHIVE_DIR="./$ARCHIVES_DEFAULT"
fi

if [ ! -d "$ARCHIVE_DIR" ]; then
    mkdir -p "$ARCHIVE_DIR"
fi

script_name=$(basename "$0")
script="${script_name%.*}"

echo "Working in directory $IMAGES_DIR, saving to directory $ARCHIVE_DIR"

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")

for file in `ls -1 $IMAGES_DIR | sed -e 's/\..*$//' | sort | uniq`; do
    if [ "$file" != "$ARCHIVES_DEFAULT" ] && [ "$file" != "$script" ] && [ ! -e "$ARCHIVE_DIR/$file.zip" ] && [ ! -d "$file" ]; then
        echo "Working with $file..."
        find "$IMAGES_DIR" -maxdepth 1 -name "$file*" ! -name "*.xpks" -print | zip "$file" -@
        mv "$file.zip" "$ARCHIVE_DIR"
    fi
done

IFS=$SAVEIFS

echo "DONE"

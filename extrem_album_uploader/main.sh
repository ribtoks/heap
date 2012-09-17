#!/bin/bash
echo "Selecting files"

zenity --file-selection --multiple > testfile

echo "Transforming lines..."

tr -s '|' '\n' < testfile > photos_to_upload
rm testfile

echo "Uploading..."

sleep 2

./extrem_album_uploader.sh

#!/bin/bash

# <bitbar.title>Sync notebooks</bitbar.title>
# <bitbar.version>v0.1</bitbar.version>
# <bitbar.author>Taras Kushnir</bitbar.author>
# <bitbar.author.github>ribtoks</bitbar.author.github>
# <bitbar.desc>This will start rclone sync</bitbar.desc>
# <bitbar.image></bitbar.image>
# <bitbar.dependencies></bitbar.dependencies>

echo "rclone"
echo "---"

REMOTE_NAME="Dropbox"
REMOTE_PATH="MyDropboxCrypt:/"
LOCAL_PATH="~/CloudSync/"

echo "↑ Sync To ${REMOTE_NAME} | terminal=true bash=\"rclone copy ${LOCAL_PATH} ${REMOTE_PATH} -P\""
echo "↓ Sync From ${REMOTE_NAME} | terminal=true bash=\"rclone copy ${REMOTE_PATH} ${LOCAL_PATH} -P\""


#!/bin/bash
PATHS_FILE=photos_to_upload
COOKIES_FILE=/tmp/extrem_cookie.file
USERNAME=ribtoks
PASSWORD=configura19ion
SECRET_STRING="j1c1aacd0f0f25c4502f69ebdfb27397b"

echo "Curl login to extrem.lviv.ua"

rm $COOKIES_FILE &> /dev/null

# login first
curl --cookie "$COOKIES_FILE" --cookie-jar "$COOKIES_FILE" \
    --data "username=$USERNAME" \
    --data "passwd=$PASSWORD" \
    --data "op2=login" \
    --data "lang=ukainian" \
    --data "force_session=1" \
    --data "message=0" \
    --data "$SECRET_STRING=1" \
    --data "return=http%3A%2F%2Fextrem.lviv.ua%2Findex.php" \
    --data "Sumbit=%C2%F5%B3%E4" \
    -L "http://extrem.lviv.ua/index.php?task=login&option=com_comprofiler" &> /dev/null

echo "Start of upload"

COUNTER=1
IMAGE_START_INDEX="$COUNTER"
TOTAL=$(wc -l $PATHS_FILE | cut -f1 -d' ')

while read file; do
    if [ -f "$file" ];
    then
        curl --cookie "$COOKIES_FILE" --cookie-jar "$COOKIES_FILE" \
            -F "imgtitle=image$IMAGE_START_INDEX" \
            -F "catid=53" \
            -F "screenshot=ON" \
            -F org_screenshot=@"$file" \
            -F "option=com_datsogallery" \
            -F "task=" -F "id=" -F "imgauthor=" -F "imgtext=" \
            -F "thumbcreation=ON" -L \
             "http://extrem.lviv.ua/index.php?option=com_datsogallery&Itemid=3&func=uploadhandler" &> /dev/null     
    fi

    echo -ne "Processed $COUNTER out of $TOTAL\r"    
    let "COUNTER += 1"
    let "IMAGE_START_INDEX += 1"
done < "$PATHS_FILE"

echo
# logout
echo "Curl logout from extrem.lviv.ua"
curl --cookie "$COOKIES_FILE" --cookie-jar "$COOKIES_FILE" \
    --data "op2=logout" \
    --data "lang=ukainian" \
    --data "force_session=1" \
    --data "message=0" \
    --data "$SECRET_STRING=1" \
    --data "return=http%3A%2F%2Fextrem.lviv.ua%2Findex.php" \
    --data "Sumbit=%C2%E8%F5%B3%E4" -L "http://extrem.lviv.ua/index.php?task=logout&option=com_comprofiler" &> /dev/null

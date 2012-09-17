#!/bin/bash
PATHS_FILE=photos_to_upload
COOKIES_FILE=/tmp/extrem_cookie.file
USERNAME=ribtoks
PASSWORD=configura19ion

# login first
curl --cookie $COOKIES_FILE --cookie-jar $COOKIES_FILE \
    --data "username=$USERNAME" \
    --data "passwd=$PASSWORD" \
    --data "op2=login" \
    --data "lang=ukainian" \
    --data "force_session=1" \
    --data "message=0" \
    --data "j9e85f56c870b8164ff1853be550f17c7=1" \
    --data "return=http%3A%2F%2Fextrem.lviv.ua%2Findex.php" \
    --data "Sumbit=%C2%F5%B3%E4" \
-L --url "http://extrem.lviv.ua/index.php?task=login&option=com_comprofiler" &> /dev/null

COUNTER=1
TOTAL=$(wc -l $PATHS_FILE | cut -f1 -d' ')

while read file; do
    if [ -f "$file" ];
    then
        curl --cookie $COOKIES_FILE --cookie-jar $COOKIES_FILE \
            -F "imgtitle=image$COUNTER" \
            -F "catid=53" \
            -F "screenshot=ON" \
            -F org_screenshot=@"$file" \
            -F "option=com_datsogallery" \
            -F "task=" -F "id=" -F "imgauthor=" -F "imgtext=" \
            -F "thumbcreation=ON" -L \
            --url "http://extrem.lviv.ua/index.php?option=com_datsogallery&Itemid=3&func=uploadhandler" &> /dev/null     
    fi

    echo -ne "Processed $COUNTER out of $TOTAL\r"    
    let "COUNTER += 1"
done < "$PATHS_FILE"

echo
# logout

curl --cookie $COOKIES_FILE --cookie-jar $COOKIES_FILE \
    --data "op2=logout" \
    --data "lang=ukainian" \
    --data "force_session=1" \
    --data "message=0" \
    --data "j9e85f56c870b8164ff1853be550f17c7=1" \
    --data "return=http%3A%2F%2Fextrem.lviv.ua%2Findex.php" \
    --data "Sumbit=%C2%E8%F5%B3%E4" -L --url "http://extrem.lviv.ua/index.php?task=logout&option=com_comprofiler" &> /dev/null

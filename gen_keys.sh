#!/bin/bash
WORKSPACE="`dirname $0`"
WORKSPACE="`readlink -f \"$WORKSPACE\"`"
if [ $WORKSPACE == /usr/bin ]; then
  WORKSPACE=$PWD
fi

KEYSPACE=$WORKSPACE/adlink/Platform/Ampere/ComHpcAltPkg/AdlinkKeys
ssh-keygen -t rsa -b 2048 -m PEM -f $KEYSPACE/Dbb_AdlinkTest.priv.pem -N ""
openssl req -new -x509 -key $KEYSPACE/Dbb_AdlinkTest.priv.pem \
                       -out $KEYSPACE/Dbb_AdlinkTest.cert.pem -days 3560 \
                       -subj "/C=TW/ST=./L=New Taipei City/O=ADLink/OU=SEC/CN=phil.xing@adlinktech.com"
ssh-keygen -t rsa -b 2048 -m PEM -f $KEYSPACE/Dbu_AdlinkTest.priv.pem -N ""
openssl req -new -x509 -key $KEYSPACE/Dbu_AdlinkTest.priv.pem \
                       -out $KEYSPACE/Dbu_AdlinkTest.cert.pem -days 3560 \
                       -subj "/C=TW/ST=./L=New Taipei City/O=ADLink/OU=SEC/CN=phil.xing@adlinktech.com"

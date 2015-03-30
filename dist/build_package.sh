#!/bin/sh

# DEBIAN
PACKAGEID="strusmodule-0.0"

cd pkg/$PACKAGEID
dpkg-buildpackage


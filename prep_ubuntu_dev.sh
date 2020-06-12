#!/bin/bash

# libgcrrypt 1.8.0 or newer (Ubuntu Bionic) is required

set -e

apt-get update
apt-get upgrade
apt-get install build-essential libgcrypt11-dev dieharder

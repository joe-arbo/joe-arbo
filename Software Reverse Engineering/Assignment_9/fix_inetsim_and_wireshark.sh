#!/bin/bash

rm -rf /opt/inetsim-1.3.2

apt update

apt install inetsim -y

chmod 4711 /usr/bin/dumpcap

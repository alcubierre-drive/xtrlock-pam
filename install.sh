#!/bin/bash

sudo cp src/xtrlock-pam /usr/bin/

echo "
You might want the following in the file ´/etc/systemd/system/suspend@.service´:

´´´
[Unit]
Description=User suspend actions
Before=suspend.target

[Service]
User=lennart
Type=simple
Environment=DISPLAY=:0
ExecStart=+/usr/bin/xtrlock-pam -s

[Install]
WantedBy=suspend.target
´´´
"

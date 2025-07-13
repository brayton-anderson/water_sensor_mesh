#!/usr/bin/env bash

cp "release/water_sensor_meshd_linux_$(uname -m)" /usr/bin/water_sensor_meshd
mkdir -p /etc/water_sensor_meshd
if [[ -f "/etc/water_sensor_meshd/config.yaml" ]]; then
	cp bin/config-dist.yaml /etc/water_sensor_meshd/config-upgrade.yaml
else
	cp bin/config-dist.yaml /etc/water_sensor_meshd/config.yaml
fi
cp bin/water_sensor_meshd.service /usr/lib/systemd/system/water_sensor_meshd.service

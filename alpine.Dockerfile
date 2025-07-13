# trunk-ignore-all(trivy/DS002): We must run as root for this container
# trunk-ignore-all(hadolint/DL3002): We must run as root for this container
# trunk-ignore-all(hadolint/DL3018): Do not pin apk package versions
# trunk-ignore-all(hadolint/DL3013): Do not pin pip package versions

FROM python:3.13-alpine3.22 AS builder
ARG PIO_ENV=native
ENV PIP_ROOT_USER_ACTION=ignore

RUN apk --no-cache add \
        bash g++ libstdc++-dev linux-headers zip git ca-certificates libgpiod-dev yaml-cpp-dev bluez-dev \
        libusb-dev i2c-tools-dev libuv-dev openssl-dev pkgconf argp-standalone \
        libx11-dev libinput-dev libxkbcommon-dev \
    && rm -rf /var/cache/apk/* \
    && pip install --no-cache-dir -U platformio \
    && mkdir /tmp/firmware

WORKDIR /tmp/firmware
COPY . /tmp/firmware

# Create small package (no debugging symbols)
# Add `argp` for musl
ENV PLATFORMIO_BUILD_FLAGS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -largp"

RUN bash ./bin/build-native.sh "$PIO_ENV" && \
    cp "/tmp/firmware/release/water_sensor_meshd_linux_$(uname -m)" "/tmp/firmware/release/water_sensor_meshd"

# ##### PRODUCTION BUILD #############

FROM alpine:3.22
LABEL org.opencontainers.image.title="Water_Sensor_Mesh" \
      org.opencontainers.image.description="Alpine Water_Sensor_Mesh daemon" \
      org.opencontainers.image.url="https://water_sensor_mesh.org" \
      org.opencontainers.image.documentation="https://water_sensor_mesh.org/docs/" \
      org.opencontainers.image.authors="Water_Sensor_Mesh" \
      org.opencontainers.image.licenses="GPL-3.0-or-later" \
      org.opencontainers.image.source="https://github.com/water_sensor_mesh/firmware/"

# nosemgrep: dockerfile.security.last-user-is-root.last-user-is-root
USER root

RUN apk --no-cache add \
        shadow libstdc++ libgpiod yaml-cpp libusb i2c-tools libuv \
        libx11 libinput libxkbcommon \
    && rm -rf /var/cache/apk/* \
    && mkdir -p /var/lib/water_sensor_meshd \
    && mkdir -p /etc/water_sensor_meshd/config.d \
    && mkdir -p /etc/water_sensor_meshd/ssl

# Fetch compiled binary from the builder
COPY --from=builder /tmp/firmware/release/water_sensor_meshd /usr/bin/
# Copy config templates
COPY ./bin/config.d /etc/water_sensor_meshd/available.d

WORKDIR /var/lib/water_sensor_meshd
VOLUME /var/lib/water_sensor_meshd

EXPOSE 4403

CMD [ "sh",  "-cx", "water_sensor_meshd --fsdir=/var/lib/water_sensor_meshd" ]

HEALTHCHECK NONE
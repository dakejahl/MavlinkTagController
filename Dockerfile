FROM arm64v8/ubuntu:focal as build-stage

ARG DEBIAN_FRONTEND=noninteractive

# Packages required to build Mavsdk
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential cmake git openssh-client \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN git config --global http.sslverify false
RUN git config --global https.sslverify false

WORKDIR /build-mavlink-cpp
RUN git clone https://github.com/DonLakeFlyer/mavlink-cpp.git
WORKDIR /build-mavlink-cpp/mavlink-cpp
RUN git checkout DonChanges
RUN git submodule update --init --recursive

RUN make all

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libusb-1.0-0-dev pkg-config git \
    python \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build-airspy
RUN git clone https://github.com/airspy/airspyhf.git
WORKDIR /build-airspy/airspyhf/build
RUN cmake ../ -DINSTALL_UDEV_RULES=ON
RUN make

WORKDIR /build-mavlink-tag-controller
COPY CMakeLists.txt .
COPY *.h .
COPY *.cpp .
ADD mavlink-headers ./mavlink-headers

WORKDIR /build-mavlink-tag-controller/build
RUN cmake .. && \
    make -j12

FROM arm64v8/ubuntu:focal as release-stage

ARG DEBIAN_FRONTEND=noninteractive

# MAVSDK dependencies
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    airspy \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

#    libjsoncpp1 \
#    libcurl4 \
#    libncurses5 \
#    libtinyxml2-6a \

WORKDIR /app
COPY --from=build-stage /build-mavlink-tag-controller/build/MavlinkTagController .
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so.0 /app/
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so /app/
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so.1.6.8 /app/
COPY --from=build-stage /build-airspy/airspyhf/build/tools/src/airspyhf_rx /app/

ENTRYPOINT [ "/app/MavlinkTagController" ]

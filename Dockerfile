########## BUILD STAGE ##########
FROM dakejahl/arm64v8-focal as build-stage

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libusb-1.0-0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


# Will run faster  if yousubmodule and copy it into the docker image
WORKDIR /build-airspy
RUN git clone https://github.com/airspy/airspyhf.git
WORKDIR /build-airspy/airspyhf/build
RUN cmake ../ -DINSTALL_UDEV_RULES=ON
RUN make

WORKDIR /build-mavlink-tag-controller

# We copy everything and then delete the build dir because there is a bug
# with Docker BuiltKit and the COPY --from=build-stage is not able to find
# the build/ directory unless we copy it in before hand.
COPY . .
RUN rm -rf build/

# COPY CMakeLists.txt .
# COPY *.h .
# COPY *.cpp .
# COPY libraries/ .

WORKDIR /build-mavlink-tag-controller/build
RUN cmake .. && \
    make -j12

########## RELEASE STAGE ##########
FROM dakejahl/arm64v8-focal as release-stage

WORKDIR /app
COPY --from=build-stage /build-mavlink-tag-controller/build/MavlinkTagController .
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so.0 /app/
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so /app/
COPY --from=build-stage /build-airspy/airspyhf/build/libairspyhf/src/libairspyhf.so.1.6.8 /app/
COPY --from=build-stage /build-airspy/airspyhf/build/tools/src/airspyhf_rx /app/

ENTRYPOINT [ "/app/MavlinkTagController" ]

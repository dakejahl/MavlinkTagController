########## BUILD STAGE ##########
FROM dakejahl/arm64v8-focal as build-stage

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libusb-1.0-0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /buildroot

# We copy everything and then delete the build dir because there is a bug
# with Docker BuiltKit and the COPY --from=build-stage is not able to find
# the build/ directory unless we copy it in before hand.
COPY . .
RUN rm -rf build/

WORKDIR /buildroot/build
RUN cmake .. && \
    make -j12

########## RELEASE STAGE ##########
FROM dakejahl/arm64v8-focal as release-stage

WORKDIR /app
COPY --from=build-stage /buildroot/build/MavlinkTagController .
COPY --from=build-stage /buildroot/build/libraries/airspyhf/tools/src/airspyhf_rx .

ENTRYPOINT [ "/app/MavlinkTagController" ]

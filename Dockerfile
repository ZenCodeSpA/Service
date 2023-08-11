FROM ubuntu:22.04

RUN apt update \
    && apt install -y wget build-essential cmake gcc make automake libssl-dev \
    && wget -q https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz \
        && tar -xf boost_1_82_0.tar.gz \
        && rm boost_1_82_0.tar.gz \
        && cd boost_1_82_0 \
        && ./bootstrap.sh toolset=clang \
        && ./b2 install release \
                        variant=release \
                        debug-symbols=on \
                        optimization=speed \
                              --with-json \
                              --with-thread \
                              --with-headers \
                              --with-coroutine \
                              --with-iostreams \
                              --with-system \
                              --with-regex \
                              --with-system \
                              --with-serialization \
                              --with-program_options \
                              --with-exception \
                              --with-contract \
                              --with-container \
                              --with-context \
                              --with-chrono \
                              --with-locale \
                              --with-thread \
                              --with-test \
                              --with-timer \
        && cd ../ \
        && rm boost_1_82_0 -Rf

WORKDIR /work

COPY . .

RUN cmake -S . -B build \
    && cd build \
    && make \
    && ./service --help


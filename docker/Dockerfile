FROM jdinius/qap-dependencies
MAINTAINER Joe Dinius

ARG username=nmsac
ENV USER=$username

# setup ldconfig to allow linking to libregistration built below
RUN echo "/or-tools_Ubuntu-18.04-64bit_v7.4.7247/lib" >> /etc/ld.so.conf.d/x86_64-linux-gnu.conf \
    && ldconfig

RUN apt-get update && apt-get install -y libprotobuf-dev \
    protobuf-compiler \
    clang \
    clang-format \
    clang-tidy \
    ninja-build \
    python3 \ 
    python3-pip \
    && pip3 install conan \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# cppcheck
RUN cd /usr/local/src \
    && wget https://github.com/danmar/cppcheck/archive/1.90.tar.gz \
    && tar xvf 1.90.tar.gz \
    && cd cppcheck-1.90 \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make install \
    && cd ../.. && rm -rf cppcheck*

# setup user env at the end
# -m option creates a fake writable home folder
RUN adduser --disabled-password --gecos '' $username
RUN adduser $username sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER $username
WORKDIR /home/$username

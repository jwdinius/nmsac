FROM jdinius/qap-dependencies-nvidia:0.3.0
MAINTAINER Joe Dinius

ARG username=nmsac
ENV USER=$username

# XXX(jwd) - not sure this is needed anymore; consider removal)
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
    lcov \
    valgrind \
    && pip3 install conan \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# INSTALL PYBIND11 (downloaded from github)
RUN wget https://github.com/pybind/pybind11/archive/v2.5.0.zip \
    && unzip v2.5.0.zip
RUN cd pybind* && mkdir build && cd build \
    && cmake .. \
    && make -j2 && make install

# INSTALL carma (from my personal fork)
RUN wget https://github.com/jwdinius/carma/archive/master.zip \
    && unzip master.zip
RUN cd carma* && mkdir build && cd build \
    && cmake .. \
    && make && make install

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

# pip install
RUN python3 --version
RUN python3 -m pip install --upgrade pip
RUN python3 -m pip install --no-cache-dir pykitti==0.3.1
RUN python3 -m pip install --no-cache-dir opencv-python==4.5.1.48

# setup user env at the end
# -m option creates a fake writable home folder
RUN adduser --disabled-password --gecos '' $username
RUN adduser $username sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER $username
WORKDIR /home/$username

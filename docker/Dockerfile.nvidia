FROM nmsac-deps

ARG username=nmsac
ENV USER=$username

# setup ldconfig to allow linking to libregistration built below
RUN echo "/or-tools_Ubuntu-18.04-64bit_v7.4.7247/lib" >> /etc/ld.so.conf.d/x86_64-linux-gnu.conf \
    && ldconfig

# setup user env at the end
# -m option creates a fake writable home folder
RUN adduser --disabled-password --gecos '' $username
RUN adduser $username sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER $username
WORKDIR /home/$username

# build binary relaxation code
RUN git clone https://github.com/jwdinius/point-registration-with-relaxation.git \
    && cd point-registration-with-relaxation \
    && mkdir build && cd build && cmake .. \
    && make -j2 \
    && sudo make install

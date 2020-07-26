rm -rf build \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make -j2 \
    && export PYTHONPATH=$(pwd)/bindings/python \
    && ctest \
    && cd .. \
    && python3 ./scripts/wrapper-test.py -s

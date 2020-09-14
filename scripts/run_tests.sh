rm -rf build \
    && mkdir build \
    && cd build \
    && cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
    && make -j2 \
    && export PYTHONPATH=$(pwd)/bindings/python \
    && ctest -j2 --output-on-failure \
    && cd .. \
    && python3 ./scripts/wrapper-test.py -s

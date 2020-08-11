rm -rf build \
    && mkdir build \
    && cd build \
    && cmake .. -DBUILD_TESTS=ON -DCODE_COVERAGE=ON \
    && make -j2 \
    && export PYTHONPATH=$(pwd)/bindings/python \
    && ctest -j2 --output-on-failure || true \
    && cd .. \
    && lcov --capture --directory ./build --output-file coverage.info \
    && lcov --remove coverage.info "/usr/*" "/or-tools*" "$(pwd)/tests/*" "$(pwd)/build/*" --output-file coverage.info &> /dev/null \
    && lcov --list coverage.info

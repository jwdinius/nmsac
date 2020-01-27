mkdir build \
    && cd build \
    && cmake -DORTOOLS_ROOT=$ORTOOLS_ROOT .. \
    && make -j2 \
    && make test

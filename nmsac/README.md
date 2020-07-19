## `nmsac::nmsac`

Implementation for Algorithm 1 (i.e. the main executive process) from the [top-level README](../README.md).  This subproject implements and extends the ideas presented in the paper [SDRSAC](https://arxiv.org/abs/1904.03483) from CVPR2019.  Most of the framework comes from the original author's [matlab implementation](https://github.com/intellhave/SDRSAC), translated into C++ and using [armadillo](http://arma.sourceforge.net/) for working with matrices and vectors.

There is a nice, end-to-end test of the `nmsac::main` algorithm [here](../tests/nmsac/main_test.cpp).

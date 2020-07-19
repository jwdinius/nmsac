# `nmsac::correspondences`
Implementation for Algorithm 2 from the [top-level README](../README.md).  For a short discussion on correspondence identification, see this [blog post](https://jwdinius.github.io/blog/2019/point-match/).

This subproject is organized in the following way:

* [`common`](./common) - common utility code and type definitions (including `CorrespondencesBase` definition, which is used to wrap all algorithm implementations for computing correspondences)
* [`qap`](./qap) - implements an optimization-based solution to the correspondences problem.
* _insert new algorithm here!  Submit a PR, if you dare!_

Each new algorithm implemented should follow the organization of the [qap](./qap) subdirectory:

* identify short, descriptive name for the implementation, i.e. `{alg}`
* create `include/{alg}` and `src` directories to hold the implementation
* create a `CMakeLists.txt` file to generate build environment for the algorithm implementation
* add appropriate unit tests in folder rooted [here](../tests)
* modify parent `CMakeLists.txt` files to make sure your implementation and tests get built

When in doubt, just follow the `qap` pattern!

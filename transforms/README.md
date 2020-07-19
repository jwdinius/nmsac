# `nmsac::transforms`

This subproject implements helper utilities for aligning point clouds after correspondences have been identified.

* [`common`](./common) - common utilities and definitions for the subproject
* [`icp` (Algorithm 3b)](./icp) - an implementation of the [Iterative Closest Point](https://en.wikipedia.org/wiki/Iterative_closest_point) algorithm that allows the user the flexibility to remove a configurable ratio of outliers
* [`svd` (Algorithm 3a)](./svd) - an implementation of [Kabsch's algorithm](https://en.wikipedia.org/wiki/Kabsch_algorithm) for finding the best rigid transformation between same-sized point sets with known correspondences

_See [top-level README](../README.md) for definitions of Algorithms 3a and 3b._

## Non-Minimal Sample Consensus _NMSAC_

_11 July 2020 update: this repo is currently under construction.  All unit tests pass (when using docker), but the documentation is currently outdated.  I am working to update over the coming weeks._

This repo is based on the paper [SDRSAC](https://arxiv.org/abs/1904.03483) from CVPR2019.  Most of the framework comes from the original author's [matlab implementation](https://github.com/intellhave/SDRSAC), translated into C++ and using [armadillo](http://arma.sourceforge.net/) for working with matrices and vectors.  At it's core, SDRSAC is about employing a sample-and-consensus strategy, like that of [RANSAC](https://en.wikipedia.org/wiki/Random_sample_consensus).  However, with non-minimal subsampling, higher quality motion hypotheses are obtained much faster than those obtained from random sampling.

Whereas SDRSAC uses _semidefinite programming_ methods to solve the correspondence problem on non-minimal subsets sampled from the source and target point clouds, NMSAC uses the approach from [here](https://github.com/jwdinius/point-registration-with-relaxation).  When trying to translate the original matlab implementation into C++, I found it difficult to find an SDP solver that could be made to work for the problem at-hand so I created my own solution using a more flexible optimization framework ([IPOPT](https://github.com/coin-or/Ipopt)).

Despite there being a solution provided for identifying correspondences from subsampled sets, the architecture for NMSAC is set up so that _any algorithm to solve the correspondence problem can be used as a drop-in replacement for the convex relaxation solver_.  Regardless of how the correspondence problem is solved, I believe that the path to obtaining higher quality motion hypotheses between point clouds is through non-minimal subsampling.

Transformation utilities, like Kabsch's algorithm and the Iterative Closest Point algorithm, are contained [here](https://github.com/jwdinius/point-registration-with-relaxation).  Check that repo's `README` for more details.

### About this Repo
The code is well-commented with unit tests provided for key functionality but some things may remain unclear.  If this is the case, please to make an issue.

#### Static code checking
For desired formatting, please see the script [linter.sh](scripts/linter.sh) and the [`cpplint` docs](https://github.com/cpplint/cpplint).

I will eventually add Travis integration to check each PR, but until then I use the linter script.
## Quick Start

The recommended approach is to use [`docker`](https://docs.docker.com/install/linux/docker-ce/ubuntu/), however I realize that not everyone is familiar with it.  For those users, check out the `RUN` steps in this [file](docker/deps/Dockerfile.std) to properly configure your workspace, including all steps needed to build dependencies from source.

### With `docker` (recommended)
#### Build images
You will first need to build the `nmsac-deps` container.  To do this, execute the following

```shell
$ cd {repo-root-dir}/docker/deps
$ #ln -s Dockerfile.std Dockerfile  ## NOTE: do this if you do not have nvidia-docker2 installed
$ ln -s Dockerfile.nvidia Dockerfile ## NOTE: do this if you do have nvidia-docker2 installed AND you want to use the nvidia runtime
$ ./build-docker.sh  ## this will take awhile to build
```

Now that you have `nmsac-deps` built locally, you can build the `nmsac` image.  This image is incremental, and basically just sets up a user environment for working with this repo.  To build the `nmsac` image, execute the following:

```shell
cd {repo-root-dir}/docker
$ ./build-docker.sh
```

#### Launch development container

```shell
cd {repo-root-dir}
$ ./docker/run-docker.sh {--runtime=nvidia}  ## only add the optional command line arg if you have the nvidia runtime available
```

You should now have an interactive shell to work from.

#### Build the library

```shell
$ cd {repo-root-dir}
$ rm -rf build && mkdir build && cd build && cmake .. && make
```
#### Test

To run the unit tests:

```shell
$ cd {repo-root-dir}/build  ## after following build steps above
$ make test
```

To see graphical output (from python 2.7):

```shell
$ cd {repo-root-dir}/build  ## after following build steps above
$ export PYTHONPATH=$(pwd):$PYTHONPATH
$ cd ../scripts
$ python wrapper-test.py -s
```

The command above sets up two cubes; one that is a translated and rotated copy of the second.  This example is provided as a simple proof-of-concept.  The user can load KITTI `*.bin` point cloud data and process it using the same `wrapper-test.py` script.  Check the options in the beginning of that script for more details.

## Sample Output (two cubes, no noise)

### Plot 1:  Unaligned Point Clouds
![](./figures/Figure_1.png)

### Plot 2: Alignment after NMSAC
![](./figures/Figure_2.png)

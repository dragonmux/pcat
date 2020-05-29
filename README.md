# pcat

[![Build Status](https://github.com/DX-MON/pcat/workflows/GitHub%20Actions/badge.svg)](https://github.com/DX-MON/pcat/actions)
[![codecov](https://codecov.io/gh/DX-MON/pcat/branch/master/graph/badge.svg)](https://codecov.io/gh/DX-MON/pcat)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/DX-MON/pcat.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/DX-MON/pcat/alerts/)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FDX-MON%2Fpcat.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FDX-MON%2Fpcat?ref=badge_shield)

pcat is a `cat`-like utility designend for fast, parallel, file concatenation, with a HPC focus in mind.

## Getting started

NB: If you did not clone `--recursive`, after cloning the respository you will need to run `git submodule init --update` to pull all dependencies for a build.
It is recomended not to use `--recursive` on this repo.

pcat is written in standards-compliant C++17 and so requires a fairly modern compiler to build.
Minimum supported compilers include: GCC 7, Clang 5, MSVC 19, and Intel ICC 19.

It also depends on [Meson](https://github.com/mesonbuild/meson) >= 0.52 and [Ninja](https://github.com/ninja-build/ninja) >= 1.9.0.

For general use the following steps are sufficient for a working build:

``` shell
meson build
ninja -C build
ninja -C build install
```


## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FDX-MON%2Fpcat.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2FDX-MON%2Fpcat?ref=badge_large)
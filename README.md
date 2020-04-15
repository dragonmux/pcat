# pcat

[![Build Status](https://github.com/DX-MON/pcat/workflows/GitHub%20Actions/badge.svg)](https://github.com/DX-MON/pcat/actions)
[![codecov](https://codecov.io/gh/DX-MON/pcat/branch/master/graph/badge.svg)](https://codecov.io/gh/DX-MON/pcat)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/DX-MON/pcat.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/DX-MON/pcat/alerts/)

pcat is a `cat`-like utility designend for fast, parallel, file concatenation, with a HPC focus in mind.

## Getting started

After cloning the respository, you will need to run `git submodule init --update` to pull all dependencies for a build.

pcat is written in standards-compliant C++17 and so requires a fairly modern compiler to build. It also depends on [Meson](https://github.com/mesonbuild/meson) >= 0.52 and [Ninja](https://github.com/ninja-build/ninja) >= 1.9.0.

Meson can be installed from `pip` if missing: `pip install meson`.
Ninja binaries are available on the Ninja releases page for Linux, Mac and Windows. If missing, please download one suitable for your OS and extract the binary somewhere in your OS's $PATH (%PATH% on Windows).

For general use the following steps are sufficient for a working build:

``` shell
meson build
ninja -C build
```

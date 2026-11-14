# MoHSES Physiology Manager

## Description
The Physiology Manager is responsible for the management of the Physiology Engine.

#### Requirements
The physiology engine requires the [MoHSES Standard Library](https://github.com/AdvancedModularManikin/amm-library) be built and available (and so requires FastRTPS and FastCDR).

In addition to the MoHSES library dependancies, the Physiology Manager also requires:
- BioGears 8.1.0 or newer - prepared packages are available: https://github.com/BioGearsEngine/core/releases/tag/8.1.0 (on Linux, build it from source instead: see [Building BioGears on Linux](#building-biogears-on-linux))


### Installation
```bash
    $ git clone https://github.com/AdvancedModularManikin/physiology-manager
    $ mkdir physiology-manager/build && cd physiology-manager/build
    $ cmake ..  
    $ cmake --build . --target install
```

### Building BioGears on Linux
The BioGears Linux install (both the prebuilt 8.1.0 and 8.2.0 packages and a source build) doesn't include the generated `biogears/schema/cdm/*.hxx` headers that the BioGears headers include, and the 8.1.0 source release doesn't configure. Build 8.2.0 from source and copy the generated headers in. Tested on Ubuntu 22.04:

```bash
    $ sudo apt install libeigen3-dev xsdcxx libxerces-c-dev file ninja-build libboost-system-dev libboost-date-time-dev libboost-filesystem-dev libboost-program-options-dev
    $ sudo ln -s /usr/bin/xsdcxx /usr/local/bin/xsd
    $ git clone --branch 8.2.0 https://github.com/BioGearsEngine/core.git biogears
    $ cmake -S biogears -B biogears/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/biogears -DBiogears_PREPARE_RUNTIME_DIR=OFF
    $ sudo cmake --build biogears/build --target install
    $ sudo cp -r biogears/build/projects/biogears/generated/Release/biogears/schema /opt/biogears/include/biogears/
    $ sudo mkdir -p /opt/biogears/share/biogears
    $ sudo cp -r biogears/share/data /opt/biogears/share/biogears/data
    $ export BIOGEARS_HOME=/opt/biogears
```

- `xsdcxx` is the Debian/Ubuntu name for the CodeSynthesis XSD compiler; the symlink lets `cmake/FindCodeSynthesis.cmake` find it.
- `file` is used by the BioGears install step.
- The BioGears Linux install doesn't include its runtime data (states, patients, substances). The copy puts it where `BiogearsConfig.cmake` expects it, so the Physiology Manager build stages it next to the executable.

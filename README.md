# MoHSES Physiology Manager

## Description
The Physiology Manager is responsible for the management of the Physiology Engine.

#### Requirements
The physiology engine requires the [MoHSES Standard Library](https://github.com/AdvancedModularManikin/amm-library) be built and available (and so requires FastRTPS and FastCDR).

In addition to the MoHSES library dependancies, the Physiology Manager also requires:
- BioGears 8.1.0 or newer - prepared packages are available: https://github.com/BioGearsEngine/core/releases/tag/8.1.0


### Installation
```bash
    $ git clone https://github.com/AdvancedModularManikin/physiology-manager
    $ mkdir physiology-manager/build && cd physiology-manager/build
    $ cmake ..  
    $ cmake --build . --target install
```

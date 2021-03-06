petalosim is a GEANT4-based software that simulates the interaction of particles in the PETALO detectors. It is based on the [nexus software](https://github.com/next-exp/nexus).

## Installation

To build petalosim, follow the same steps to build the `nexus` software, described in the [nexus twiki](https://github.com/next-exp/nexus/wiki/Installing-and-running-nexus).
The only addition is the dependency on the `nexus` code itself. Make sure to have:
1. The nexus bin directory in your PATH variable.
2. The nexus lib directory in your LD_LIBRARY_PATH (or DYLD_LIBRARY_PATH for macOS systems) variable.

## Run

In order to run petalosim, be sure to execute the setup script, placed in the `scripts` folder.

* If you use a macOS more recent than Mojave (version >= 10.15), you should execute:

`source /path/to/scripts/petalo_setup.zsh`

* For the rest of operative systems:

`source /path/to/scripts/petalo_setup.sh`

It will set the `PETALODIR` variable to the top level directory of the installation.

To run petalosim, just type

```
bin/petalo -b -n numb_of_events init_macro_filename
```


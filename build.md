# Building

## Windows

### Install MinGW

1. Download the installer.  
2. Install it to the default paths.  
2.1 Default paths will be assumed. Otherwise, just replace occurences with your chosen path.  

### Install GNU Arm Embedded Toolchain

1. Download the installer.  
2. Install it to the default paths.  
2.1 Default paths will be assumed. Otherwise, just replace occurences with your chosen path.  

### Update PATH variables

1. `Windows Control Panel -> System -> Advanced System Settings -> Advanced -> Environment Variables ...`  
2. Under System variables, if "PATH" exists, double click it. Otherwise, create it.  
3. If the value of the variable is shown as a single line, append `;<MinGW Root>\bin;<GNU Arm Embedded Root>\bin;`.
If it is shown as a list, append the new entries `<MinGW Root>\bin` and `<GNU Arm Embedded Root>\bin`.
`<MinGW Root>` and `<GNU Arm Embedded Root>` being the locations you installed the MinGW and GNU Arm Embedded toolchain to.  

### Actually building

1. Open command prompt.  
2. `cd` into the TMC-EvalShield root folder.  
3. Enter `mingw32-make`.  
4. Binaries should be in the build directory.  

## Linux

This requires your favorite package manager to be set up. On debian and its derivatives,
`[sudo] apt-get install <package name>` can be used to install the required packages.

1. Install `build-essential` package.  
2. Install `gcc-arm-none-eabi` package.  
3. `cd` into the TMC-EvalShield root folder.  
4. Run `make`.  
5. Binaries should be in the build directory.  
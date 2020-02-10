# TMC-EvalShield

## Abstract
The TMC-EvalShield is the TRINAMIC Evaluation Firmware for STMicroelectronics STM32-Nucleo boards.  
This aims to simplify the evaluation process of our motion controller shields.  

## Current state
Currently only Nucleo STM32F103XX is supported.

## Setup
1. **Cloning**  
1.1 **HTTPS**  
`git clone --recurse-submodules https://github.com/trinamic/TMC-EvalShield.git`  
1.2 **SSH**  
1.2.1 **Clone everything with HTTPS**  
`git clone --recurse-submodules https://github.com/trinamic/TMC-EvalShield.git`  
1.2.2 **Update root and submodules to use SSH**  
```shell
git remote set-url origin git@github.com:trinamic/TMC-EvalShield.git
cd TMC-API
git remote set-url origin git@github.com:trinamic/TMC-API.git
cd ..
```  
2. **Building**  
2.1 **Command Line**  
To build this project in command line, a simple `cd <project root>` and `make` should do the job.  
This requires paths to be set correctly.  
2.2 **Eclipse**  
To build this project in Eclipse IDE simply import the project via `File -> Import -> General -> Existing projects into workspace`.  
After that, hit the build button to build the target *Default*, which should invoke the included Makefile.  
Eventually, you have to adjust the paths according to your environment.

## Documentation
For documentation, read the file doc.md.
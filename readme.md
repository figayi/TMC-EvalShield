# TMC-EvalShield

## Abstract
The TMC-EvalShield is the TRINAMIC Evaluation Firmware for STMicroelectronics STM32-Nucleo boards.  
This aims to simplify the evaluation process of our motion controller shields.  

## Current state
Currently only Nucleo STM32F103XX is supported.  
Currently only TMC5160-SHIELD is supported.

## Setup

### Cloning

#### HTTP

`git clone --recurse-submodules https://github.com/trinamic/TMC-EvalShield.git`  

#### SSH

1. Clone with https  
`git clone --recurse-submodules https://github.com/trinamic/TMC-EvalShield.git`
2. Update root and submodules to use SSH
```shell
git remote set-url origin git@github.com:trinamic/TMC-EvalShield.git
cd TMC-API
git remote set-url origin git@github.com:trinamic/TMC-API.git
cd ..
```  

### Building

For a build guide, read build.md.

## Documentation
For documentation, read doc.md.

## Operational guide
For an operational guide showing how to get started with the basics, read guide.md.
# Pi-chuck
This project of mine aims to compute pi using a serie by the chudnosky brothers.
It uses a custom Decimal Arbitrary precision Int (DAI) library to perform these calculations

## Warning
As this project is still in its birth, nothing is functionnal, compiling and running it will only print example calculations as part of testing DAI

## Quick start
to build and run with custom DAI (experimental):
```
$ make
```
to build with gmp : 
```
$ make gmp
```
Just keep in mind you need gmp to use this

to regenerate the pdf from the latex:
```
$ make latex
```
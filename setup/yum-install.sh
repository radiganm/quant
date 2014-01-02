#!/bin/bash
## yum-install.sh
## Mac Radigan

## This is a list of project dependencies, defined in terms of a yum repository.

yum -y install cmake28*
yum -y install maven2
yum -y install glibc
yum -y install gcc
yum -y install gcc-c++
yum -y install glibc-static
yum -y install java
yum -y install java-1.6.0-openjdk
yum -y install boost
yum -y install root*
yum -y install octave octave-forge
yum -y install fftw
yum -y install atlas
yum -y install lapack
yum -y install blas
yum -y install gnuplot
yum -y install maven2
yum -y install gnuplot gnuplot-common gnuplot-doc* gnuplot-latex* 
yum -y install curl
yum -y install armadillo
yum -y install liblog4cxx liblog4cxx-devel

## *EOF*

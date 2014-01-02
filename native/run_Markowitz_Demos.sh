#!/bin/bash

file=${0##*/}
dir=${0%/*}
root=${dir}/..

(cd $root; mkdir -p results/portfolios)

# Portfolio #1
(cd $root; ./native/Portfolio/bin/markowitz -f ./resources/portfolios/portfolio-AAPL_JPM_LMT_XOM-25.xml)

# Portfolio #2
(cd $root; ./native/Portfolio/bin/markowitz -f ./resources/portfolios/portfolio-ABT_BA_CVX_GE-25.xml)

# Portfolio #3
(cd $root; ./native/Portfolio/bin/markowitz -f ./resources/portfolios/portfolio-AMZN_ORCL_SO-20.xml)

# *EOF*

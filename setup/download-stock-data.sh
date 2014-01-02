#!/bin/bash
# Mac Radigan

##  s – Symbol
##  n – Name
##  l – Last Trade (with time)
##  l1 – Last Trade (without time)
##  d1 – Last Trade Date
##  t1 – Last Trade Time
##  k3 – Last Trade Size
##  c – Change and Percent Change
##  c1 – Change
##  p2 – Change in Percent
##  t7 – Ticker Trend
##  v – Volume
##  a2 – Average Daily Volume
##  i – More Info
##  t6 – Trade Links
##  b – Bid
##  b6 – Bid Size
##  a – Ask
##  a5 – Ask Size
##  p – Previous Close
##  o – Open
##  m – Day’s Range
##  w – 52 Week Range
##  j5 – Change from 52 Week Low
##  j6 – Percent Change from 52 Week Low
##  k4 – Change from 52 Week High
##  k5 – Percent Change from 52 Week High
##  e – Earnings/Share
##  r – P/E Ratio
##  s7 – Short Ratio
##  r1 – Dividend Pay Date
##  q – Ex-Dividend Date
##  d – Dividend/Share
##  y – Dividend Yield
##  f6 – Float Shares
##  j1 – Market Capitalization
##  t8 – 1 Year Target Price
##  e7 – EPS Est. Current Year
##  e8 – EPS Est. Next Year
##  e9 – EPS Est. Next Quarter
##  r6 – Price/EPS Est. Current Year
##  r7 – Price/EPS Est. Next Year
##  r5 – PEG Ratio
##  b4 – Book Value
##  p6 – Price/Book
##  p5 – Price/Sales
##  j4 – EBITDA
##  m3 – 50 Day Moving Average
##  m7 – Change from 50 Day Moving Average
##  m8 – Percent Change from 50 Day Moving Average
##  m4 – 200 Day Moving Average
##  m5 – Change from 200 Day Moving Average
##  m6 – Percent Change from 200 Day Moving Average
##  s1 – Shares Owned
##  p1 – Price Paid
##  c3 – Commission
##  v1 – Holdings Value
##  w1 – Day’s Value Change
##  g1 – Holdings Gain Percent
##  g4 – Holdings Gain
##  d2 – Trade Date
##  g3 – Annualized Gain
##  l2 – High Limit
##  l3 – Low Limit
##  n4 – Notes
##  k1 – Last Trade (Real-time) with Time
##  b3 – Bid (Real-time)
##  b2 – Ask (Real-time)
##  k2 – Change Percent (Real-time)
##  c6 – Change (Real-time)
##  v7 – Holdings Value (Real-time)
##  w4 – Day’s Value Change (Real-time)
##  g5 – Holdings Gain Percent (Real-time)
##  g6 – Holdings Gain (Real-time)
##  m2 – Day’s Range (Real-time)
##  j3 – Market Cap (Real-time)
##  r2 – P/E (Real-time)
##  c8 – After Hours Change (Real-time)
##  i5 – Order Book (Real-time)
##  x – Stock Exchange

function download {
  sym=$1
  # 2010-01-01 to 2013-01-31
  sd=1
  sm=1
  sy=2010
  ed=31
  em=12
  ey=2013
  url="http://ichart.finance.yahoo.com/table.csv?s=${sym}&d=${sd}&e=${ed}&f=${ey}&g=d&a=0&b=${sd}&c=${sy}&ignore=.csv"
  echo $url
  dir=data/${sym}
  mkdir -p ${dir}
  file=${dir}/${sym}.yqd
  echo "quote> downloading ${sym} to ${file} (${url})"
  echo $url > ${file}
  curl -s "$url" >> ${file}
  file=${dir}/${sym}.csv
  curl -s "$url" > ${file}
}

file=${0##*/}
dir=${0%/*}

symbols=` grep -Ev "^#.*"  symbols.list `
for s in $symbols
do
  (cd ${dir}/..; download $s)
done

## *EOF*

// Series.cxx
// Mac Radigan

#include "Series.hxx"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <boost/foreach.hpp>

USING_QUANT
using namespace std;

Series::Series() 
{
}

Series::~Series() 
{
}

Series::Series(const Series &series)
{
  symbol    = series.symbol;
  date      = series.date;
  close     = series.close;
  open      = series.open;
  high      = series.high;
  low       = series.low;
  volume    = series.volume;
  adj_close = series.adj_close;
  rreturn   = series.rreturn;
}

int Series::operator<(const Series &rhs) const
{
  if( this->date < rhs.date ) return 1;
  return 0;
}


Series& Series::operator=(const Series &rhs)
{
  this->symbol    = rhs.symbol;
  this->date      = rhs.date;
  this->close     = rhs.close;
  this->open      = rhs.open;
  this->high      = rhs.high;
  this->low       = rhs.low;
  this->volume    = rhs.volume;
  this->adj_close = rhs.adj_close;
  this->rreturn   = rhs.rreturn;
  return *this;
}

int Series::operator==(const Series &rhs) const
{
  if( this->symbol    != rhs.symbol ) return 0;
  if( this->date      != rhs.date ) return 0;
  if( this->close     != rhs.close ) return 0;
  if( this->open      != rhs.open ) return 0;
  if( this->high      != rhs.high ) return 0;
  if( this->low       != rhs.low ) return 0;
  if( this->volume    != rhs.volume ) return 0;
  if( this->adj_close != rhs.adj_close ) return 0;
  if( this->rreturn   != rhs.rreturn ) return 0;
  return 1;
}

string Series::getAsCsv(int nsamples) const 
{
  stringstream ss;
  for(int idx=1; idx<nsamples; idx++) 
  {
    char strdate[11];
    time_t t = date.at(idx);
    strftime(strdate,11,"%Y-%m-%d",localtime(&t));
    ss << strdate << ","
       << close.at(idx) << ","
       << open.at(idx) << ","
       << high.at(idx) << ","
       << low.at(idx) << ","
       << volume.at(idx) << ","
       << adj_close.at(idx)
       << endl;
  }
  return *(new string(ss.str()));
}

void Series::load(string symbol, string filename) 
{
  this->symbol = symbol;
  vector<string> results;
  string line;
  string header;
  ifstream file(filename.c_str());
  if(!file.is_open()) {
    string msg = "Unable to open file: ";
    msg+=filename;
    throw runtime_error(msg);
  }
  // allocate buffers for reading
  char  *b_date = (char*)calloc(11,sizeof(char));
  double b_close;
  double b_open;
  double b_high;
  double b_low;
  int    b_volume;
  double b_adj_close;
  double b_rreturn;
  getline(file,header);
  while(getline(file,line)) 
  {
    sscanf(line.c_str(),"%10c,%lf,%lf,%lf,%lf,%d,%lf\n",
      b_date,&b_open,&b_high,&b_low,&b_close,&b_volume,&b_adj_close);
    struct tm t;
    strptime(b_date,"%Y-%m-%d",&t);
    date.push_back(mktime(&t));
    open.push_back(b_open);
    high.push_back(b_high);
    low.push_back(b_low);
    close.push_back(b_close);
    volume.push_back(b_volume);
    adj_close.push_back(b_adj_close);
    /* fprintf(stdout,"%s\t%f\t%f,%f\t%f\t%d\t%f\n",
         b_date,b_close,b_open,b_high,b_low,b_volume,b_adj_close); */
  }
  // rate of return (daily):
  //   r[n] = ( c[n]-c[n-1] ) / c[n-1]  with c closing price
  for(int idx=0; idx<close.size()-1; idx++) 
  {
    rreturn.push_back( (close.at(idx)-close.at(idx+1))/close.at(idx+1) );
  }
  file.close();
}

// *EOF*

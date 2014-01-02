// Series.hxx
// Mac Radigan
//
// Description:  This class is a parser and container for 
//               financial time series data.
//
//               In the current implementation, historical 
//               financial time series are read from a 
//               Yahoo! Finance comma-separated values file (CSV).
//
// See Also:     http://finance.yahoo.com
//               for more information on Yahoo! Finance data sources
//

#include "quant.hxx"
#include <string>
#include <iostream>
#include <vector>
#include <time.h>

#ifndef SERIES_HXX
#define SERIES_HXX

NS_QUANT_BEGIN

class Series 
{
  private:
    std::string              symbol;
    std::vector<time_t>      date;
    std::vector<double>      close;
    std::vector<double>      open;
    std::vector<double>      high;
    std::vector<double>      low;
    std::vector<int>         volume;
    std::vector<double>      adj_close;
    std::vector<double>      rreturn;
  protected:
  public:
    Series(); 
    ~Series(); 
    void load(std::string symbol, std::string filename); 
    //inline std::vector<std::string> getDate() { return date; }
    inline std::vector<time_t>      getDate() { return date; }
    inline std::vector<double>      getClose() { return close; }
    inline std::vector<double>      getOpen() { return open; }
    inline std::vector<double>      getHigh() { return high; }
    inline std::vector<double>      getLow() { return low; }
    inline std::vector<int>         getVolume() { return volume; }
    inline std::vector<double>      getAdjClose() { return adj_close; }
    inline std::vector<double>      getRreturn() { return rreturn; }
    std::string getAsCsv(int nsamples) const; // serialize data to Comma Separated Value (CSV) format
    Series& operator=(const Series &rhs);
    Series(const Series &copyin);
    int operator==(const Series &rhs) const;
    int operator<(const Series &rhs) const;
    //friend ostream &operator<<(ostream &, const Series &);
};

NS_QUANT_END

#endif

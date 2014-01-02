// Figure.hxx
// Mac Radigan
//
// Description:  This class is used for plotting figures.
//               It is a pipe interface to gnuplot, which 
//               must be installed and available on the 
//               environment search path to function correctly.
//
// See Also:     http://www.gnuplot.info
//               for more information on gnuplot
//

#include "quant.hxx"
#include "Series.hxx"
#include "TMatrixD.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <boost/foreach.hpp>

#ifndef FIGURE_HXX
#define FIGURE_HXX

NS_QUANT_BEGIN

class Figure 
{
  private:
    bool debug;
    static const char* defaultName;
    const double* const getData(TMatrixD& mat);
    const double* const getOrdinalRange(TMatrixD& mat);
  protected:
    FILE* gpin;
    static const int stringBufferSize;
    std::string name;
    std::stringstream script;
    std::string title;
    std::string xlabel;
    std::string ylabel;
    std::string format;
    std::string terminal;
    std::string output;
    double pause;
    void initialize();
    void dispose();
    friend inline std::ostream& operator<<(std::ostream& os, const Figure& fig) 
    {
      os << fig.toString();
      return os;
    }
  public:
    Figure(); 
    Figure(const NS_QUANT::Figure& fig) { this->gpin=fig.gpin; };
    ~Figure(); 
    inline void setTitle(std::string title) { this->title=title; }
    void show();
    inline void clear() { std::cout<<std::string(100,'\n'); }
    inline void setName(std::string name) { this->name=name; }
    inline void setFormat(std::string format) { this->name=name; }
    void save(std::string fieldname);
    void plotCandle(Series& series, const int nsamples);
    void plotPortfolio(std::vector<std::string>& symbols, 
                       std::vector<double>& roi, 
                       std::vector<double>& std,
                       double roi_opt,
                       double std_opt);
    inline void setTerminal(const char* const p) { terminal=p; }
    inline void setOutput(const char* const p) { output=p; }
    inline void setYlabel(const char* const p) { ylabel=p; }
    inline void setXlabel(const char* const p) { xlabel=p; }
    inline void setTitle(const char* const p) { title=p; }
    inline void setPause(double t) { pause=t; }
    void reset();
    inline std::string& toString() const {
      std::string *pstr = new std::string(script.str());
      return *pstr;
    }
};

NS_QUANT_END

#endif

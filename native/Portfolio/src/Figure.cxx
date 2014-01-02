// Figure.cxx
// Mac Radigan

#include "Figure.hxx"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <strings.h>
#include <stdlib.h>
#include <stdexcept>
#include <limits>
#include <math.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#undef GNUPLOT_CANDLE_GREEN_RED

USING_QUANT
using namespace std;
using namespace boost;
using namespace boost::filesystem;

const int Figure::stringBufferSize = 1024;

Figure::Figure() {
  initialize();
}

Figure::~Figure() { 
  dispose();
}

void Figure::initialize() {
  pause = 0.0;
  name = "default";
  gpin = popen("gnuplot","w");
  reset();
}

void Figure::dispose() {
  pclose(gpin);
}

void Figure::reset() {
  script.str(string());
  script << "set datafile commentschars \"#%\"" << endl;
  setTerminal("dumb");
  setOutput("");
  setTitle("unnamed");
  setXlabel("x");
  setYlabel("y");
}

const double* const Figure::getData(TMatrixD& mat) {
   const double* const dvalue = mat.GetMatrixArray();
   return dvalue;
}

const double* const Figure::getOrdinalRange(TMatrixD& mat) {
   long long length = mat.GetNcols() * mat.GetNrows();
   double* dbuffer = (double*)calloc(length,sizeof(long long));
   for(long long idx=0; idx<length; idx++) dbuffer[idx] = idx;
   const double* const dvalue = static_cast<const double* const>(dbuffer);
   return dvalue;
}

void Figure::show() {
  fprintf(gpin, "set terminal %s\n", terminal.c_str());
  if(!output.empty()) {
    fprintf(gpin, "set output %s\n", output.c_str());
  }
  fprintf(gpin, "set title \"%s\"\n", title.c_str());
  fprintf(gpin, "set xlabel \"%s\"\n", xlabel.c_str());
  fprintf(gpin, "set ylabel \"%s\"\n", ylabel.c_str());
  fprintf(gpin, "%s\n", script.str().c_str());
  if(pause>0.0) {
     cerr << "pause " << pause << " seconds" << endl;
     fprintf(gpin, "pause(%g)\n", pause);
  }
  fflush(gpin);
}

void Figure::plotPortfolio(vector<std::string>& symbols, 
                           vector<double>& roi, 
                           vector<double>& std,
                           double roi_opt,
                           double std_opt) {
  script << "set datafile separator ','" << endl;
  script << "set multiplot" << endl;
  script << "set xlabel '% Risk (annualized)' offset 1" << endl;
  script << "set ylabel '% Return (annualized)' offset 1" << endl;
  script << "set border linewidth 1.5" << endl;
  script << "set pointsize 2.5" << endl;
  script << "set style line 1 lc rgb '#00FF00' pt 9 # red triangle" << endl;
  script << "set style line 2 lc rgb '#FF0000' pt 9 # green triangle" << endl;
  script << "set yrange [0:40]" << endl;
  script << "set xrange [0:40]" << endl;
  //script << "set grid" << endl;
  script << "set lmargin 9" << endl;
  script << "set rmargin 2" << endl;
  script << "set multiplot" << endl;
  script << "set key off" << endl;
  script << "plot '-' w p ls 2" << endl;
  typedef vector<string> formats_vector;
  for(int sIdx=0; sIdx<symbols.size(); sIdx++) {
    script << std.at(sIdx) << "," << roi.at(sIdx) << endl;
  }
  script << "e" << endl;
  script << "plot '-' using 1:2:3 with labels offset 1.5,1.5 notitle" << endl;
  for(int sIdx=0; sIdx<symbols.size(); sIdx++) {
    script << std.at(sIdx) << "," << roi.at(sIdx) 
           << "," << symbols.at(sIdx) << endl;
  }
  script << "e" << endl;
  script << "plot '-' w p ls 1" << endl;
  script << std_opt << "," << roi_opt << endl;
  script << "e" << endl;
  script << "plot '-' using 1:2:3 with labels offset 1.5,1.5 notitle" << endl;
  script << std_opt << "," << roi_opt << "," << "efficient" << endl;
  script << "e" << endl;
  script << "unset multiplot" << endl;
}

void Figure::plotCandle(Series& series, const int nsamples) {
  vector<double> high = series.getHigh();
  vector<double> low  = series.getLow();
  double minPrice = numeric_limits<double>::max();
  double maxPrice = numeric_limits<double>::min();
  for(int idx=1; idx<nsamples; idx++) {
    if(low.at(idx)<minPrice) minPrice = low.at(idx);
    if(high.at(idx)>maxPrice) maxPrice = high.at(idx);
  }
  script << "# format: Date,Open,High,Low,Close,Volume,Adj Close" << endl;
  script << "set datafile separator ','" << endl;
  script << "set yrange [" 
         << floorf(minPrice) << ":" 
         << ceilf(maxPrice) << "]" << endl;
  script << "set grid" << endl;
  script << "set lmargin 9" << endl;
  script << "set rmargin 2" << endl;
  script << "set multiplot" << endl;
  script << "set size 1, 0.7" << endl;
  script << "set origin 0, 0.3" << endl;
  script << "set bmargin 0" << endl;
  script << "set xdata time" << endl;
  script << "set timefmt '%Y-%m-%d'" << endl;
  script << "set ylabel 'price (USD)' offset 1" << endl;
  script << "set format x '%b''%y'" << endl;
  script << "set autoscale xfix" << endl;
  //script << "set boxwidth 5.0" << endl;
  script << "set bars 2.0" << endl;
  script << "set style fill empty" << endl;
  // Date,Open,High,Low,Close,Volume,Adj Close
  // 1    2    3    4   5     6      7
#ifdef GNUPLOT_CANDLE_GREEN_RED
  script << "plot '-' every ::2 using 1:($5<=$2?$2:1/0):($5<=$2?$3:1/0):($5<=$2?$4:1/0):($5<=$2?$5:1/0) notitle with candlesticks lt rgb '#00FF00'" 
         << ", "
         << "'-' every ::2 using 1:($5>$2?$2:1/0):($5>$2?$3:1/0):($5>$2?$4:1/0):($5>$2?$5:1/0) notitle with candlesticks lt rgb '#FF0000'" 
         << endl;
  script << series.getAsCsv(nsamples);
  script << "e" << endl;
#else
  script << "plot '-' using 1:2:3:4:5 notitle with candlesticks" << endl;
  script << series.getAsCsv(nsamples);
  script << "e" << endl;
#endif
  script << "unset title" << endl;
  script << "set bmargin" << endl;
  script << "set size 1.0, 0.3" << endl;
  script << "set origin 0.0, 0.0" << endl;
  script << "set tmargin 0" << endl;
  script << "set autoscale y" << endl;
  script << "set format y '%1.0f'" << endl;
  script << "set ylabel 'volume (x 1k)' offset 1" << endl;
  script << "plot '-' using 1:($6/10000) notitle with impulses lt 3" << endl;
  script << series.getAsCsv(nsamples);
  script << "e" << endl;
  script << "unset multiplot" << endl;
}

void Figure::save(string filename) {
  path file(filename);
  string dir = path(file.parent_path()).string();
  string ext = path(file.extension()).string();
  //if(!exists(dir)) {
  //  cerr << "mkdir> " << dir << endl;
  //  mkdir(dir.c_str(), PERM_DIRECTORY);
  //}
  if(!ext.compare(".gnuplot")) {
    //cerr << "output type set to gnuplot" << endl;
    ofstream fout;
    fout.open(filename.c_str());
    fout << "#!/usr/bin/env gnuplot" << endl;
    fout << "set terminal X11 persist" << endl;
    fout << "#set output \"" << output.c_str() << "\"" << endl;
    fout << "set title \"" << title.c_str() << "\"" << endl;
    fout << "set xlabel \"" << xlabel.c_str() << "\"" << endl;
    fout << "set ylabel \"" << ylabel.c_str() << "\"" << endl;
    fout << script.str().c_str() << endl;
    fout.close();
  } else {
    string tempTerminal = terminal;
    string tempOutput = output;
    if(!ext.compare(".ps")) {
      //cerr << "output type set to postscript" << endl;
      setTerminal("postscript enhanced color portrait");
    } else if(!ext.compare(".eps")) {
      //cerr << "output type set to postscript enhanced color" << endl;
      setTerminal("postscript eps enhanced color dashed");
    } else if(!ext.compare(".png")) {
      //cerr << "output type set to portable networks graphics" << endl;
      setTerminal("png");
    } else if(!ext.compare(".tif")) {
      //cerr << "output type set to tagged image format" << endl;
      setTerminal("tiff");
    } else if(!ext.compare(".jpg")) {
      //cerr << "output type set to joint photographic experts group" << endl;
      setTerminal("jpeg");
    } else if(!ext.compare(".pdf")) {
      //cerr << "output type set to portable document format" << endl;
      setTerminal("pdf");
    } else {
      throw runtime_error("Unsupported output file extension.");
    }
    string newOutput = "\"" + filename + "\"";
    setOutput(newOutput.c_str());
    cerr << "save> " << filename << endl;
    show();
    setTerminal(tempTerminal.c_str());
    setOutput(tempOutput.c_str());
  }
}

// *EOF*

// Portfolio.cxx
// Mac Radigan

#include "Portfolio.hxx"
#include "Figure.hxx"
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <vector>
#include <math.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

USING_QUANT
using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::algorithm;

// window size (in days) of stock price samples
const int Portfolio::WINDOW_LENGTH = 250; 
// number of trading days in a year (time horizon)
const int Portfolio::TIME_HORIZON = 250;  

Portfolio::Portfolio(string datapath) 
{
  this->datapath = datapath;
  isOptimized = true;
}

Portfolio::~Portfolio() 
{ 
}

mat& Portfolio::getReturnsAsMatrix() 
{
  // convert portfolio returns to matrix format
  //
  // x is an M x N matrix of daily returns, 
  //    where M is the number daily returns
  //    and   N is the number number of stocks in the portfolio
  int np = stocks.size();   // number of stocks in portfolio
  int ns = WINDOW_LENGTH;   // number of samples
  mat *x = new mat(ns,np);
  int sIdx = 0;
  int nIdx = 0;
  typedef map<string,Series> stock_map;
  BOOST_FOREACH(stock_map::value_type& it, stocks) 
  {
    nIdx=0;
    vector<double> rreturn = it.second.getRreturn();
    for(int nIdx=0; nIdx<min((int)rreturn.size(),ns); nIdx++) 
    {
      (*x)(nIdx,sIdx) = rreturn.at(nIdx); // rates of return (daily)
    }
    sIdx++;
  }
  return *x;
}

void Portfolio::optimize(double rreturn_opt) 
{
  // x is an M x N matrix of daily returns, 
  //    where M is the number daily returns
  //    and   N is the number number of stocks in the portfolio
  mat x = getReturnsAsMatrix();
  // s is a (M x N) variance-covariance matrix (of portfolio daily returns)
  mat s = cov(x);
  // mu is a (1 x M) matrix of mean portfolio daily returns
  //   mu = mean(x)
  mat mu = mean(x);
  // volatility is a (1 x M) matrix of portfolio daily standard deviation
  //   volatility = sqrt(diag(cov))
  mat sigma = sqrt(s.diag()).t();
  // convert target return to fractional daily
  double mu_opt = rreturn_opt/(TIME_HORIZON*100); 
  // save for the portfolio
  rreturn.resize(mu.n_rows, mu.n_cols);          rreturn=mu;
  volatility.resize(sigma.n_rows, sigma.n_cols); volatility=sigma; 
  //
  // special case:  If there is only one stock in the portfolio,
  //                the matrix solution representation is A_3x3 
  //                singular matrix.  Handle a single-investment
  //                portfolio as a special case;
  //
  if(1==stocks.size()) 
  {
    if(rreturn(0,0)!=rreturn_opt) 
    {
       // a single-stock portfolio has only one possible return
       // (if not allowing for short-selling, or inclusion of a risk-free asset)
       throw runtime_error("Portfolio cannot achieve specified estimated target ROI.");
    }
    // the return from 1 stock (annualized and reported as a percentage)
    portfolio_rreturn    = mu_opt;
    // the volatility of 1 stock (annualized and reported as a percentage)
    portfolio_volatility = volatility(0,0)/sqrt(1/TIME_HORIZON)*100;
    // the single investment weight is unity [1]
    weights.resize(1,1); weights(0,0) = 1;
    return;
  }
  //
  /*
   * Markowitz portfolio optimization with constraint on ROI
   *
   *      minimize:  sigma_p,w^2 = w'*S*w 
   *
   *      subject to:     mu_opt = w'*mu
   *
   *                      w'*1v  = 1
   *
   *      where
   *         sigma_p,w^2      portfolio variances
   *         mu_opt  target   portfolio rate of return
   *         w                portfolio weights
   *         S                covariance matrix
   *         1v               one vector
   *
   *
   *           A       *     x       =     b
   *     | 2S  mu 1v | * | w       | = | 0v     |
   *     | mu' 0  0  |   | lambda1 |   | mu_opt |
   *     | 1'  0  0  |   | lambda2 |   | 1      |
   *
   *       where
   *         S       daily rate of return covariance matrix
   *         mu      daily mean ROIs
   *         mu_opt  target portfolio rate of return
   *         w       portfolio weights
   *         1v      one vector (size of portfolio)
   *         0v      zero vector (size of portfolio)
   *         lambda1 Lagrange multiplier for target return
   *         lambda2 Lagrange multiplier for normalized weight constraint
   */
  // Markowitz linear equations
  int np = s.n_rows;
  mat muT = mu.t();          // mu'
  mat v1  = ones<mat>(np,1); // v1
  mat v1T = ones<mat>(1,np); // v1'
  //
  //     [ 2S   mu v1 ]
  // A = [ mu'  0  0  ]
  //     [ v1'  0  0  ]
  //
  mat a = zeros<mat>(np+2, np+2);
  for(int r=0; r<s.n_rows; r++) 
  {
    for(int c=0; c<s.n_cols; c++) 
    {
      a(r,c) = 2*s(r,c);
    }
  }
  for(int idx=0; idx<np; idx++) 
  {
    a(idx,np)   = mu(0,idx);  // mu
    a(np,idx)   = mu(0,idx);  // mu
    a(idx,np+1) = 1.0;        // 1
    a(np+1,idx) = 1.0;        // 1
  }
  //
  //     [ 0v     ]
  // b = [ mu_opt ]
  //     [ 1      ]
  //
  mat b(np+2,1);
  b(np,0)   = mu_opt;                     // mu_opt
  b(np+1,0) = 1.0;                        // 1
  //
  // Az=b  -->  z = A^-1*b
  //
  mat z = solve(a,b); // solve Ax=b
  // first 1..Np elements are the weights
  mat wT = z.rows(0,np-1);
  mat w = wT.t();
  weights = zeros<mat>(np,1);
  weights = w;
  // results:
  // portfolio variance
  mat sig2_opt = w*s*wT; 
  // portfolio standard deviation
  mat sig_opt = sqrt(sig2_opt);
  // portfolio volatility
  //   mat mu_opt_sol = w*muT;
  //   portfolio_rreturn = annualize(mu_opt_sol(0,0));
  portfolio_rreturn = rreturn_opt;
  // annualized as a percentage
  //   sig_a = sig/sqrt(1/T)*100
  portfolio_volatility = 
    sig_opt(0,0)/sqrt(1/static_cast<double>(TIME_HORIZON))*100; 
  // annualized portfolio returns
  //   rreturn = ((1+mu).^T-1)*100;     
  rreturn = annualizeRate(mu);
  // annualized portfolio volatilities
  //   volatility = sig/sqrt(1/T)*100
  volatility = sigma;
  volatility *= (1/sqrt(1/static_cast<double>(TIME_HORIZON)))*100; 
  isOptimized = true; // flag the calculation
}

mat& Portfolio::dailyRate(mat& x) 
{
  //
  // annualized percentage:
  //
  //   x = (1+x_a/100).^1/T-1;
  //
  //     where 
  //       x_a is matrix of annual samples matrix
  //       T   is the annual number of trading days
  //
  int m = x.n_rows;
  int n = x.n_cols;
  mat *b = new mat(m,n);
  for(int row=0; row<m; row++) 
  {
    for(int col=0; col<n; col++) 
    {
      (*b)(row,col) = dailyRate(x(row,col));
    }
  }
  return *b;
}

double Portfolio::dailyRate(double x) 
{
  //
  // daily percentage:
  //
  //   x = (1+x_a/100).^T-1
  //
  //     where 
  //       x is matrix of daily samples matrix
  //       T is the annual number of trading days
  //
  return ::pow(1.0+static_cast<long double>(x)/100.0,
          1.0/static_cast<long double>(TIME_HORIZON))-1;
}

mat& Portfolio::annualizeRate(mat& x) 
{
  //
  // annualized percentage:
  //
  //   x_a = ((1+x).^T-1)*100
  //
  //     where 
  //       x is matrix of daily samples matrix
  //       T is the annual number of trading days
  //
  int m = x.n_rows;
  int n = x.n_cols;
  mat *b = new mat(m,n);
  for(int row=0; row<m; row++) 
  {
    for(int col=0; col<n; col++) 
    {
      (*b)(row,col) = annualizeRate(x(row,col));
    }
  }
  return *b;
}

double Portfolio::annualizeRate(double x) 
{
  //
  // annualized percentage:
  //
  //   x_a = ((1+x).^T-1)*100
  //
  //     where 
  //       x is matrix of daily samples matrix
  //       T is the annual number of trading days
  //
  return (::pow(1.0+static_cast<long double>(x),
          static_cast<long double>(TIME_HORIZON))-1)
          *100.0;
}

void Portfolio::addSeries(string symbol) 
{
  //Series *series = new Series();
  Series series;
  string filename = datapath + "/";
  filename += symbol + "/" + symbol + ".csv";
  series.load(symbol,filename);
  stocks.insert(pair<string,Series&>(symbol,series));
  isOptimized = false;
}

void Portfolio::createReport(string directory) 
{
  vector<string> formats;
  formats.push_back("gnuplot");
  formats.push_back("dumb");
  formats.push_back("eps");
  formats.push_back("jpg");
  //formats.push_back("pdf");
  //formats.push_back("tif");
  formats.push_back("png");
  typedef vector<string> formats_vector;
  typedef map<string,Series> stock_map;
  vector<string> symbols;
  vector<double> roi;
  vector<double> std;
  int sIdx = 0;
  BOOST_FOREACH(const stock_map::value_type sit, stocks) 
  {
    std.push_back(volatility(0,sIdx));
    roi.push_back(rreturn(0,sIdx));
    symbols.push_back(sit.first);
    sIdx++;
  }
  string name = join(symbols,"_");
  stringstream rootdir;
  rootdir << directory << "/" << name;
  create_directory(rootdir.str());
  BOOST_FOREACH(formats_vector::value_type& fmt, formats) {
    BOOST_FOREACH(stock_map::value_type& sit, stocks) {
    // candle plots
      Figure figure;
      figure.setTerminal("dumb");
      figure.setTitle(sit.first);
      figure.setXlabel("");
      figure.setYlabel("");
      figure.plotCandle(sit.second, WINDOW_LENGTH);
      if(iequals(fmt,"dumb")) {
        figure.show();
      } else {
        stringstream filename;
        filename << rootdir.str() << "/" << sit.first << "." << fmt;
        figure.save(filename.str());
      }
      figure.reset();
    }
    // risk/return plots
    Figure figure;
    figure.setTerminal("dumb");
    figure.setTitle("Portfolio Performance");
    figure.plotPortfolio(symbols, roi, std,
                         portfolio_rreturn, portfolio_volatility);
    if(iequals(fmt,"dumb")) {
      figure.show();
    } else {
      stringstream filename;
      filename << rootdir.str() << "/Portfolio" << "." << fmt;
      figure.save(filename.str());
    }
    figure.reset();
  }
}

string& Portfolio::toString() const 
{
  stringstream ss;
  if(!isOptimized) 
  {
    ss << "arbitrary portfolio:  target return has not been specified" 
       << std::endl;
  } else {
    ss << setiosflags(ios::fixed) << setprecision(3)
       << "efficient portfolio: " 
       << "return=" << portfolio_rreturn << "%, "
       << "volatility=" << portfolio_volatility << "%"
       << endl;
    ss << endl;
    ss << "\tSYMBOL\tWEIGHT\tRETURN\t VOLATILITY" << endl;
    int sIdx = 0;
    typedef map<string,Series> stock_map;
    BOOST_FOREACH(const stock_map::value_type& it, stocks) {
      string symbol(it.first);
      ss << setiosflags(ios::fixed) 
         << "\t" << symbol 
         << "\t" 
         << setprecision(3) << setfill('0') << setw(5) 
         << weights(0,sIdx)
         << "\t" 
         << setprecision(3) << setfill('0') << setw(6) 
         << rreturn(0,sIdx) 
         << "%"
         << "\t " 
         << setprecision(3) << setfill('0') << setw(6) 
         << volatility(0,sIdx) 
         << "%"
         << endl;
      sIdx++;
    }
  }
  return *(new string(ss.str()));
}

// *EOF*

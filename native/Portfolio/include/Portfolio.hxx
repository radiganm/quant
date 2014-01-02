// Portfolio.hxx
// Mac Radigan
//
// Description:  This class identifies the minimum-variance
//               Markowitz portfolio, for a specified set of
//               portfolio stocks and target portfolio return 
//               on investment.
//
// See Also:     http://wikipedia.org/wiki/Modern_portfolio_theory
//               for more information on the Markowitz portfolio
//

#include "quant.hxx"
#include "Series.hxx"
#include <armadillo>
#include <string>
#include <iostream>
#include <map>

#ifndef PORTFOLIO_HXX
#define PORTFOLIO_HXX

NS_QUANT_BEGIN

using namespace arma;

class Portfolio 
{
  private:
    // window size (in days) of stock price samples
    static const int WINDOW_LENGTH; 
    // number of trading days in a year (time horizon)
    static const int TIME_HORIZON;  
    std::string datapath;                // path do stock data directory
    std::map<std::string,Series> stocks; // stocks(ticker symbol, time series)
    mat weights;                    // portfolio weights (sum to 1)
    mat volatility;                 // individual volatilities
    mat rreturn;                    // individual returns
    double portfolio_rreturn;            // portfolio return
    double portfolio_volatility;         // portfolio volatility
    bool isOptimized;                    // dirty record flag
    // convert to annualized percentage
    static mat& annualizeRate(mat& x);
    static double annualizeRate(double x);
    // convert to daily percentage
    static mat& dailyRate(mat& x);
    static double dailyRate(double x);
    mat& getReturnsAsMatrix();       // convert portfolio returns
                                          // to matrix format
    // copy constructor is not implemented, restrict use as private
    Portfolio(const Portfolio& portfolio) {};
    Portfolio& operator=(const Portfolio& portfolio) { return *this; };
    friend std::ostream& operator<<(std::ostream& os, const Portfolio& p);
  protected:
  public:
    Portfolio(std::string datapath); 
    ~Portfolio(); 
    void addSeries(std::string symbol); 
    void createReport(std::string directory); 
    void optimize(double rreturn_opt);
    inline double getPortfolioReturn() { return portfolio_rreturn; };
    inline double getPortfolioVolatility() { return portfolio_volatility; };
    inline mat& getReturn() { return rreturn; };
    inline mat& getVolatility() { return volatility; };
    std::string& toString() const;
};

std::ostream& operator<<(std::ostream& os, const Portfolio& p) 
{
  os << p.toString();
  return os;
}

NS_QUANT_END

#endif

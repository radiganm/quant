% MARKOWITZPORTFOLIO minimum-variance Markowitz portfolio solver
%
%  [weights, volatility, volatility_opt, portfolio_rreturn, rate_of_return] ...
%    = markowitzPortfolio(symbols, window_length, return_opt);
%  
%    where 
%            symbol          is a cell array of ticker symbols,
%                            i.e. {symbol1, symbol2, ...}
%
%            window_length   is the number of samples used in
%                            risk/return estimation
%
%            return_opt      is the desired annual portfolio percent return
%
%            weights         is a Nx1 real matrix of the Markowitz portfolio 
%                            allocation weights, one for each ticker symbol
%
%            weights         is a Nx1 real matrix of the Markowitz portfolio 
%                            allocation weights, one for each ticker symbol
%
%            volatilities    is a Nx1 real matrix of the volatility of
%                            asset in the portfolio, one for each ticker symbol
%
%            volatility_opt  is the volatility of the Markowitz-weighted
%                            portfolio
%
%            rate_of_return  is a Nx1 real matrix of annualized rates of return
%                            of each asset in the portfolio, one for each
%                            ticker symbol
%
%  Description:  This class identifies the minimum-variance
%                Markowitz portfolio, for a specified set of
%                portfolio stocks and target portfolio return 
%                on investment.
% 
%  Examples:
%
%    return_opt    = 25.0;     % desired annualized rate of return (percent)
%    window_length = 250;      % window size (in days) of stock price samples
%                              % 250 = 1 year of trading days
%    symbols = sort({    ...   % stocks in portfolio
%                'AAPL', ...
%                'JNJ',  ...
%                'LMT',  ...
%                'XOM',  ...
%               })';
%
%    [weights, volatility, volatility_opt, rate_of_return] ...
%        = markowitzPortfolio(symbols, window_length, return_opt);
%
%
%  See Also:     http://wikipedia.org/wiki/Modern_portfolio_theory
%                for more information on the Markowitz portfolio
% 
%  Author:
%    Mac Radigan

function [weights, volatility, volatility_opt, portfolio_rreturn, rate_of_return, S, R] = markowitzPortfolio(symbols, window_length, return_opt=NaN)
  volatility = [];        % volatility
  rate_of_return = [];    % rates of return
  weights = [];           % portfolio weights
  volatility_opt = NaN;   % portfolio volatility
  %
  T = 250;   % number of trading days in a year (time horizon)
             % (either 250 or 252, depending specific reporting requirements)
  P = 1/T;   % time period
  Np = numel(symbols);         % number of stocks in portfolio
  Ns = window_length-1;        % number of samples
  % X is an M x N matrix of daily returns, 
  %    where M is the number daily returns
  %    and   N is the number number of stocks in the portfolio
  X = zeros(Ns,Np);
  for idx=1:numel(symbols)
    % retreive stock data
    ds = getStockData(symbols{idx});
    ds2.(symbols{idx}) = ds;
    x_close   = ds.Close(1:window_length);        % limit samples to window_length
    X(:,idx)  = -1*diff(x_close)./x_close(2:end); % rates of return (daily)
  end
  R = corrcoef(X);      % correlation matrix of daily returns
  S = cov(X,1);         % covariance matrix of daily returns
  mu = mean(X);         % average daily rate of return
  sig = sqrt(diag(S))'; % variance of daily return
  %
  % If a target rate of return is specified, perform a perfolio optimization with
  % this constraint.  Otherwise, use the global minimum variance solution.
  %
  if(isnan(return_opt)) % global minimum-variance Markowitz portfolio
    [w, mu_opt, sig_opt] = MPglobalMinimumVariance(mu, Np, S);
  else                  % Markowitz portfolio with specified rate of return
    [w, mu_opt, sig_opt] = MPconstrainedReturn(mu, S, Np, T, return_opt);
  end
  % return values: portfolio weights, portfolio volatility, and
  %                individual rates of return and volatilities
  % retport all return/volatility as annualized percentages
  weights = w;                               % rename rates
  volatility = sig/sqrt(1/T)*100;            % annualized volatility (%)
  volatility_opt = sig_opt/sqrt(1/T)*100;    % annualized portfolio variance (%)
  rate_of_return = ((1+mu).^T-1)*100;        % annualized portfolio 
                                             % rate of return (%)
  portfolio_rreturn = ((1+mu_opt).^T-1)*100; % annualized target return (%)
end

function [w, mu_opt, sig_opt] = MPglobalMinimumVariance(mu, Np, S)
  % Markowitz global minimum-variance portfolio optimization
  %
  %      minimize:    sigma_p,w^2 = w'*S*w 
  %
  %      subject to:        w'*1v = 1
  %
  %      where
  %         sigma_p,w^2    portfolio variances
  %         w              portfolio weights
  %         S              covariance matrix
  %         1v             one vector
  %
  %
  %         A     *      x      =    b
  %     | 2S 1v | * | w       | = | 0v  |
  %     | 1' 0  |   | lambda2 |   | 1   |
  %
  %       where
  %         S       daily rate of return covariance matrix
  %         S       daily rate of return covariance matrix
  %         mu      daily mean ROI
  %         w       portfolio weights
  %         1v      one vector (size of portfolio)
  %         0v      zero vector (size of portfolio)
  %         lambda2 Lagrange multiplier for normalized weight constraint
  %
  % Markowitz linear equations
  A = [ 2*S ones(1,Np)' ; ones(1,Np) 0 ];
  b = [ zeros(1,Np) 1 ]';
  z = A\b;                   % solve Ax=b
  w = z(1:Np);               % assign portfolio weights
  mu_opt   = mu*w;           % mean portfolio return
  sig2_opt = w'*S*w;         % portfolio variance
  sig_opt  = sqrt(sig2_opt); % portfolio standard deviation
end

function [w, mu_opt, sig_opt] = MPconstrainedReturn(mu, S, Np, T, return_opt)
  % Markowitz portfolio optimization with constraint on ROI
  %
  %      minimize:  sigma_p,w^2 = w'*S*w 
  %
  %      subject to:     mu_opt = w'*mu
  %
  %                      w'*1v  = 1
  %
  %      where
  %         sigma_p,w^2      portfolio variances
  %         mu_opt  target   portfolio rate of return
  %         w                portfolio weights
  %         S                covariance matrix
  %         1v               one vector
  %
  %
  %           A       *     x       =     b
  %     | 2S  mu 1v | * | w       | = | 0v     |
  %     | mu' 0  0  |   | lambda1 |   | mu_opt |
  %     | 1'  0  0  |   | lambda2 |   | 1      |
  %
  %       where
  %         S       daily rate of return covariance matrix
  %         mu      daily mean ROIs
  %         mu_opt  target portfolio rate of return
  %         w       portfolio weights
  %         1v      one vector (size of portfolio)
  %         0v      zero vector (size of portfolio)
  %         lambda1 Lagrange multiplier for target return
  %         lambda2 Lagrange multiplier for normalized weight constraint
  %
  mu_opt = (1+return_opt/100)^(1/T)-1; % convert target return to fractional daily
  % Markowitz linear equations
  A = [ 2*S mu' ones(1,Np)' ; mu 0 0 ; ones(1,Np) 0 0 ];
  b = [ zeros(1,Np) mu_opt 1 ]';
  z = A\b;                         % solve Ax=b
  w = z(1:Np);                     % assign portfolio weights
  mu_opt_assert  = mu*w;           % mean portfolio return (sanity check)
  sig2_opt       = w'*S*w;         % portfolio variance
  sig_opt        = sqrt(sig2_opt); % portfolio standard deviation
  % check result
  tol = 10*eps;                           % numerical tolerance
  assert(abs(mu_opt_assert-mu_opt)<tol);  % assert
  A
  b
  z
end

%% *EOF*

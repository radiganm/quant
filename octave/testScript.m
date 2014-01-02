#!/usr/bin/env octave
#!/usr/bin/env octave -qf

return_opt    = 25.0;     % desired annualized rate of return (percent)
window_length = 250;      % window size (in days) of stock price samples
                          % 250 = 1 year of trading days
symbols = sort({    ...   % stocks in portfolio
            'AAPL', ...
            'JNJ',  ...
            'LMT',  ...
            'XOM',  ...
           })';
SAVE_FIGURES = false;      % enable to save figures {T:enable, F:disable}

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
  R = corrcoef(X)       % correlation matrix of daily returns
  S = cov(X,1)          % covariance matrix of daily returns
  mu = mean(X)          % average daily rate of return
  sig = sqrt(diag(S))'  % variance of daily return

[weights, volatility, volatility_opt, rate_of_return] ...
  = markowitzPortfolio(symbols, window_length, return_opt);

if(0)

% print the efficient portfolio results as a table
fprintf(stdout, 'efficient portfolio: return=%2.2f%%, volatility=%2.2f%%\n',return_opt,volatility_opt);
% load all tabular data into a cell array
D=vertcat( ... 
  symbols', num2cell(weights)', ...
  num2cell(rate_of_return), num2cell(volatility)); 
fprintf(stdout, '\tSYM\tWEIGHT\tRETURN\tVOLATILITY\n',D{:});  % print tabular data
fprintf(stdout, '\t%s\t%2.2f\t%2.2f%%\t%2.2f%%\n',D{:});  % print tabular data
end

% plot results
if(SAVE_FIGURES)
  FS=15;
  FW='bold';
  mxy = [max([volatility volatility_opt]), max([rate_of_return return_opt])];
  dxy = 1.5*[1 1];
  %hfig = figure('visible','off');
  hfig = figure('visible','on');
  scatter(volatility,rate_of_return,40,'red','^');
  hold on;
  text(volatility+dxy(1),rate_of_return+dxy(2),symbols);
  scatter(volatility_opt,return_opt,40,'green','^');
  text(volatility_opt+dxy(1),return_opt+dxy(2),'efficient');
  title('Portfolio Performance','FontSize',FS,'fontweight',FW);
  ylabel('Annualized Return (\mu)','FontSize',FS,'fontweight',FW);
  xlabel('Annualized Risk (\sigma)','FontSize',FS,'fontweight',FW);
  legend('Efficient Portfolio');
  axis([0 mxy(1)],[0 mxy(2)]);
  outdir='results'; 
  fmts={'eps','jpg','png','tiff'}; 
  name=sprintf('portfolio_performance_%s',strjoin('_',symbols));
  mkdir(outdir);
  for fmt=fmts
    filename=sprintf('%s/%s.%s',outdir,name,fmt{:});
    fprintf(stdout,'\tsave> %s\n',filename);
    saveas(hfig,filename);
  end
end

%% *EOF*

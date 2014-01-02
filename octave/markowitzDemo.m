#!/usr/bin/env octave
%#!/usr/bin/env octave -qf
% MARKOWITZDEMO  runs the Markowitz demo
%
%  Description:  This is a command line entry point for
%                demonstrating portfolio balancing using 
%                a basic applicaton Markowitz portfolio theory.
%
%                In the current implementation, a target return 
%                on investments is specified, short selling is 
%                permitted, and there is no inclusion of a risk-free
%                asset.
%
%  See Also:     http://wikipedia.org/wiki/Modern_portfolio_theory
%                for more information on the Markowitz portfolio
%
%                http://finance.yahoo.com
%                for more information on Yahoo! Finance data sources
%
%  Author:
%    Mac Radigan

return_opt    = 25.0;     % desired annualized rate of return (percent)
window_length = 250;      % window size (in days) of stock price samples
                          % 250 = 1 year of trading days
symbols = sort({    ...   % stocks in portfolio
            'AAPL', ...
            'JNJ',  ...
            'LMT',  ...
            'XOM',  ...
           })';
SAVE_FIGURES = true;      % enable to save figures {T:enable, F:disable}

[weights, volatility, volatility_opt, portfolio_rreturn, rate_of_return] ...
  = markowitzPortfolio(symbols, window_length, return_opt);

% print the efficient portfolio results as a table
fprintf(stdout, 'efficient portfolio: return=%2.2f%%, volatility=%2.2f%%\n',portfolio_rreturn,volatility_opt);
% load all tabular data into a cell array
D=vertcat( ... 
  symbols', num2cell(weights)', ...
  num2cell(rate_of_return), num2cell(volatility)); 
fprintf(stdout, '\tSYM\tWEIGHT\tRETURN\tVOLATILITY\n',D{:});  % print tabular data
fprintf(stdout, '\t%s\t%2.2f\t%2.2f%%\t%2.2f%%\n',D{:});  % print tabular data

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

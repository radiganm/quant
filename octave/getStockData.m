function [data] = getStockData(symbol)
% GETSTOCKDATA read Yahoo! Finance time series CSV files
%
%  getStockData(symbol)
%  
%    where 
%            symbol is a ticker symbol (character array)
%
%            data   is a time-series structure with the 
%                   following fields:
%                     .symbol    - ticker symbol          (character array)
%                     .Date      - epoch time in seconds  (Nx1 int32 matrix)
%                     .DateStr   - localized date string  (character array)
%                     .Open      - opening price          (Nx1 real matrix)
%                     .High      - high price             (Nx1 real matrix)
%                     .Low       - low price              (Nx1 real matrix)
%                     .Close     - closing price          (Nx1 real matrix)
%                     .Volume    - volume                 (Nx1 real matrix)
%                     .Adj_Close - adjusted closing price (Nx1 real matrix)
%
%  Examples:
%    data = getStockData('LMT')
%
%  Author:
%    Mac Radigan
% 
  data = {};                                             % stock data
  data.symbol = symbol;                                  % stock ticker symbol
  filename = sprintf('../data/%s/%s.csv',symbol,symbol); % path to stock data
  if(~exist(filename,'file'))
    error('file %s not found',filename);                 % assert
  end
  fid = fopen(filename,'r');                             % open for reading
  fields = regexprep(strsplit(fgetl(fid),','),' ','_');  % read field names
  fmt = '%10c,%f,%f,%f,%f,%d,%f\n';                      % CSV field pattern
  n = size(strsplit(fmt,','),2);                         % number of fields
  D = cell(n,1);                                         % temporary storage
  for fIdx=1:n, data.(fields{fIdx}) = []; end            % initialize fields
  while(!feof(fid))
   [D{:},~] = fscanf(fid,fmt,'C');                         % read each line
   data.(fields{1}){end+1} = ...
     mktime(strptime(D{1},'%Y-%m-%d'));                    % record datetime
   for fIdx=2:n, data.(fields{fIdx})(end+1) = D{fIdx}; end % assign fields
  endwhile
  % all matrix operations are defined in terms of column vectors, 
  % so transpose
  for fIdx=2:n, data.(fields{fIdx}) = data.(fields{fIdx})'; end
  % convert datetime stamp to ASCII string, using system localization
  for rIdx=2:numel(data.Date)
    data.DateStr{rIdx} = asctime(localtime(data.Date{rIdx})); % localized date
  end
  fclose(fid);
end

%% *EOF*

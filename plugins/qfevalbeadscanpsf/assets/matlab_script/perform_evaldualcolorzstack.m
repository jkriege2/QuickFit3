%%
%% B040 SPIM Beadscan evaluation scripts
%%  (C) 2011-2013 Jan W. Krieger <j.krieger@dkfz.de, jan@jkrieger.de>
%%
%% This file is part of B040 SPIM Beadscan evaluation scripts.
%%
%% B040 SPIM Beadscan evaluation scripts is free software: you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation, either version 3 of the License, or
%% (at your option) any later version.
%%
%% B040 SPIM Beadscan evaluation scripts is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
%% GNU General Public License for more details.
%%
%% You should have received a copy of the GNU General Public License
%% along with Copyright Header. If not, see <http:%%www.gnu.org/licenses/>.
%%

function [ output_args ] = perform_evaldualcolorzstack( datal, datar, psf_positions, output_prefix, varargin )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

  cnt=1;
  if (~isempty(datal))
    output_args{cnt}=perform_evalzstack(datal, psf_positions, [output_prefix 'sub1_'], varargin{1:length(varargin)});
    cnt=cnt+1;
  end
  
  if (~isempty(datar))
      output_args{cnt}=perform_evalzstack(datar, psf_positions, [output_prefix 'sub2_'], varargin{1:length(varargin)});
      cnt=cnt+1;
  end
end


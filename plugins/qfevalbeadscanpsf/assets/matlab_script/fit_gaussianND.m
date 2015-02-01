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

function [ g ] = fit_gaussianND( params, data, datasize, cscale, cshift )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
dimensions=length(datasize);
dm=reshape(data, datasize);

if (dimensions==1)
    x=((1:datasize(1))-cshift(1))*cscale(1);
    x0=params(1);
    covariance=params(2);
    offset=params(3);
    amplitude=params(4);
    for x=1:datasize(1)
        xx=(x-cshift(1))*cscale(1);
        g(x)=GaussianND( xx, x0, covariance, offset, amplitude);
    end
elseif(dimensions==2)
    x=((1:datasize(1))-cshift(1))*cscale(1);
    g(1:datasize(1),1:datasize(2))=0;
    x0=params(1:2);
    covariance=params(3:6);
    offset=params(7);
    amplitude=params(8);
    for y=1:datasize(2)
        for x=1:datasize(1)
            xx=(x-cshift(1))*cscale(1);
            yy=(y-cshift(2))*cscale(2);
            g(x,y)=GaussianND( [xx yy], x0, covariance, offset, amplitude);
        end
    end
elseif(dimensions==3)
    
else
    error('function fit_gaussianND() only supports up to 3 dimensions');
end
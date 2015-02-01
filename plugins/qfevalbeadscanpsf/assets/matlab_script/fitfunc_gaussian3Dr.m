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

function [ result ] = fitfunc_gaussian3Dr( params, data)

datasize=size(data);

g(1:datasize(1),1:datasize(2),1:datasize(3))=0;
offset=params(1);
amplitude=params(2);
x0=params(3:5);
sigma=params(6:8);
theta=params(9);
phi=params(10);
alpha=params(11);


[X,Y,Z] = meshgrid(1:datasize(2),1:datasize(1),1:datasize(3));

g=Gaussian3Drmg( X,Y,Z, offset, amplitude, x0, sigma, theta, phi, alpha);

result=reshape(data-g, 1,datasize(1)*datasize(2)*datasize(3));
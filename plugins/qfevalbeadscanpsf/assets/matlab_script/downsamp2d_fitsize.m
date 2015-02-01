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

function M=downsamp2d_fitsize(M,bindims)
%DOWNSAMP2D_FITSIZE - simple tool for 2D downsampling, crops the input
%                     image befor downsizing
%
%  M=downsamp2d_fitsize(M,bindims)
%
%in:
%
% M: a matrix
% bindims: a vector [p,q] specifying pxq downsampling
%
%out:
%
% M: the downsized matrix
%
% taken from: https://groups.google.com/forum/?fromgroups#!topic/comp.soft-sys.matlab/hcJeflz9NYk[1-25]

p=bindims(1); q=bindims(2);
[m,n]=size(M); %M is the original matrix

nm=floor(m/p)*p;
nn=floor(n/q)*q;

M=M(1:nm,1:nn);
[m,n]=size(M); %M is the original matrix

M=sum(  reshape(M,p,[]) ,1 );
M=reshape(M,m/p,[]).'; %Note transpose

M=sum( reshape(M,q,[]) ,1);
M=reshape(M,n/q,[]).'; %Note transpose

M=M/(p*q);
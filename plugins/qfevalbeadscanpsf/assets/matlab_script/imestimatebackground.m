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

function [ background ] = imestimatebackground( image, varargin )
%imestimatebackground(image) tries to estimate the average background greyvalue of a given n-dimensional image
%
% This function takes the corners ( cornerfraction*length pixels ) of the
% whole image and from these average the lower averagefraction pixels to
% yield the average background value.
% 
% The parameters default values are:
%   cornerfraction=0.1
%   averagefraction=0.75
%
% You can set these parameters using the syntax:
%   imestimatebackground( image, 'param1', value1, 'param2', value2, ...)

    cornerfraction=0.1;
    averagefraction=0.75;

    optargin = size(varargin,2);
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'cornerfraction') )
            cornerfraction=val;
        elseif (strcmp(name, 'averagefraction') )
            averagefraction=val;
        else
            error(['unknown parameter "' name '" to imestimatebackground()']);
        end
    end

    is=size(image);
    idim=length(is);
    ipixels=length(image(:));
    X=cell(idim,1);
    [X{:}]=ind2sub(is, 1:ipixels);
    X=cell2mat(X);
    imin=cornerfraction*size(image);
    imax=(1-cornerfraction)*size(image);
    Xb=true(1,ipixels);
    for d=1:idim
        Xb=Xb& ( (X(d,:)<=imin(d)) | (X(d,:)>=imax(d)) );
    end
    
    back=sort(image(Xb));
    back=back(~isnan(back));
    background=mean(back(1:int32(length(back)*averagefraction)));
end


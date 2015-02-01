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

function [ com ] = imcom( image, varargin )
%imcom(image) calculate the center of mass/center of gravity of the given n-D image

    is=size(image);
    w=ones(1, length(is));

    optargin = size(varargin,2);
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'w') )
            w=val;
        end
    end
    
    
    isum=sum(image(~isnan(image(:))));
    
    X=cell(length(is),1);
    [X{:}]=ind2sub(is, 1:length(image(:)));
    for cnt=1:length(X)
        X{cnt}=(X{cnt}-1)*w(cnt);
    end
    X=cell2mat(X);
    
    image(isnan(image(:)))=0;
    com=X*image(:)/isum;

end


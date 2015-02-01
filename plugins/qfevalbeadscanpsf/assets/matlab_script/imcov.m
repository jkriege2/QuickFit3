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

function [ c, varargout ] = imcov( image, varargin )
%imcov(image) calculates the covariance matrix of the pixel coordinates, weighted with the image
%
%   The result of this function allows to estimate the image centroid.
%
%   c=imcov(image) returns the covariance matrix in c, which has size n*n
%                  for an n-dimentional image
%   [c, com]=imcov(image) returns the covariance matrix in c as above and
%                         also the center of mass of the image
%
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
    
    com=imcom(image, 'w', w);

    idim=length(is);
    isum=sum(image(~isnan(image(:))));
    ilen=length(image(:));
    image(isnan(image(:)))=0;
    X=cell(idim,1);
    [X{:}]=ind2sub(is, 1:ilen);
    for cnt=1:length(X)
        X{cnt}=(X{cnt}-1)*w(cnt);
    end
    X=cell2mat(X);
    comX=repmat(com,1,ilen);
    % calculate variances as diagonal elements of the output
    c=diag((X-comX).^2*image(:)/isum);

    % calculate the covariances:
    for idx=1:idim
        for idy=idx+1:idim
            cc=((X(idx,:)-comX(idx)).*(X(idy,:)-comX(idy)))*image(:)/isum;
            c(idx, idy)=cc;
            c(idy, idx)=cc;
        end
    end
    
    nout = max(nargout,1)-1;
    if (nout>0)
        varargout{1}=com;
    end

end


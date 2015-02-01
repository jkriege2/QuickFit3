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

function [ x0, sigmas, angle, offset, amplitude ] = performfit_gaussian2Dr( data, varargin )
  
    optargin = size(varargin,2);
    
    datasize=size(data);
    anglerange=[-pi/4 pi/4];

    sx0=datasize./2;
    [C,I]=max(data(:));
    if length(I)>0
        [sx0(1) sx0(2)]=ind2sub(datasize, I(1));
    end
    
    %sx0
    ssigmas=datasize./10;
    sangle=0;
    soffset=min(min(data));
    samplitude=max(max(data))-soffset;
    
         w=[1 1];
    
    
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'w') )
            w=val;
        end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % estimate parameters of gaussian using the covariance matrix
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % 1. estimate background
    backval=imestimatebackground(data);

    % 2. estimate center-of-mass and cov matrix of the image
    [cv, com]=imcov(data-backval, 'w', w);
    [iV, iD]=eig(cv);
    [iD, iX]=sort(diag(iD), 'descend');
    iV(iX,:)=iV(1:ndims(data),:);
    niV=iV;
    niV(1,:)=iV(1,:)/norm(iV(1,:));
    niV(2,:)=iV(2,:)/norm(iV(2,:));
    sx0=com;
    ssigmas=sqrt(iD);
    sangle=atan2(iV(1,1), iV(1,2))+pi/2;
    
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'sx0') )
            sx0=val;
        elseif (strcmp(name, 'ssigmas') )
            ssigmas=val;
        elseif (strcmp(name, 'sangle') )
            sangle=val;
        elseif (strcmp(name, 'soffset') )
            soffset=val;
        elseif (strcmp(name, 'samplitude') )
            samplitude=val;
        elseif (strcmp(name, 'anglerange') )
            anglerange=val;
        end
    end
    
    
    p0=[ soffset samplitude sx0(1) sx0(2) ssigmas(1) ssigmas(2) sangle];
    
    ff= @(p)fitfunc_gaussian2Dr_w( p, data, w);
    
    options=optimset('lsqnonlin');
    options = optimset(options,'MaxFunEvals',10000);
    options = optimset(options,'MaxIter',5000);
    options = optimset(options,'Display','off');
    
    pout=lsqnonlin(ff, p0, [-1e20 -1e20 -1e20 -1e20 1e-20 1e-20 anglerange(1)], [1e20 1e20 1e20 1e20 1e20 1e20 anglerange(2)], options);
    
    
    offset=pout(1);
    amplitude=pout(2);
    x0=pout(3:4);
    sigmas=pout(5:6);
    angle=pout(7);
end


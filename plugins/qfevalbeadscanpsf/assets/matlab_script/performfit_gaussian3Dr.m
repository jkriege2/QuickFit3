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

function [ x0, sigmas, theta, phi, alpha, offset, amplitude ] = performfit_gaussian3Dr( data, varargin )
  
    optargin = size(varargin,2);
    
    datasize=size(data);

    sx0=datasize./2;
    [C,I]=max(data(:));
    if length(I)>0
        [sx0(1) sx0(2) sx0(3)]=ind2sub(datasize, I(1));
    end
    
    %sx0
    ssigmas=datasize./10;
    stheta=0;
    sphi=0;
    alpha=0;
    soffset=min(data(:));
    samplitude=max(data(:))-soffset;
    verbose_out=false;
    MaxFunEvals=5000;
    MaxIter=1000;
    
     w=[1 1 1];
    
    
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
    niV(3,:)=iV(3,:)/norm(iV(3,:));
    sx0=com;
    ssigmas=sqrt(iD);
    [stheta, sphi]=directionToSphericalAngles([niV(1,2) niV(1,1) niV(1,3)]);
    [eer, eetheta, eephi]=sphericalCoordinatesUnitVectors(stheta,sphi);
    salpha=acos([niV(2,2) niV(2,1) niV(2,3)]*eetheta');
    if (salpha>pi/2)
        salpha=salpha-pi;
    end
    if (salpha<-pi/2)
        salpha=salpha+pi;
    end
    soffset=backval;
    samplitude=max(data(:))-soffset;

    
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'MaxFunEvals') )
            MaxFunEvals=val;
        elseif (strcmp(name, 'MaxIter') )
            MaxIter=val;
        elseif (strcmp(name, 'verbose') )
            verbose_out=val;        
        elseif (strcmp(name, 'init') )
            sx0=val(1:3);
            ssigmas=val(4:6);
            stheta=val(7);
            sphi=val(8);
            salpha=val(9);
            soffset=val(10);
            samplitude=val(11);
        end
    end
    
    
    p0=[ soffset samplitude sx0(2) sx0(1) sx0(3) ssigmas(1) ssigmas(2) ssigmas(3) stheta sphi salpha];
    
    ff= @(p)fitfunc_gaussian3Dr_w( p, data, w);
    
    options=optimset('lsqnonlin');
    options = optimset(options,'MaxFunEvals',MaxFunEvals);
    options = optimset(options,'MaxIter',MaxIter);
    if verbose_out
        options = optimset(options,'Display','on');
    else
        options = optimset(options,'Display','off');
    end
    
    pout=lsqnonlin(ff, p0, [-1e20 -1e20 -1e20 -1e20 -1e20 1e-20 1e-20 1e-20 -pi -pi/2 0], [1e20 1e20 1e20 1e20 1e20 1e20 1e20 1e20 pi pi/2 pi/2], options);
    pout=lsqnonlin(ff, pout, [-1e20 -1e20 -1e20 -1e20 -1e20 1e-20 1e-20 1e-20 -pi -pi/2 0], [1e20 1e20 1e20 1e20 1e20 1e20 1e20 1e20 pi pi/2 pi/2], options);
    
    
    offset=pout(1);
    amplitude=pout(2);
    x0=pout(3:5);
    sigmas=pout(6:8);
    theta=pout(9);
    phi=pout(10);
    alpha=pout(11);
    if verbose_out
        disp([' fit final: x0        = [' num2str(x0) ']     (initial: [' num2str(sx0') '] )']);
        disp([' fit final: sigma     = [' num2str(sigmas) ']     (initial: [' num2str(ssigmas') '] )']);
        disp([' fit final: offset    = [' num2str(offset) ']     (initial: [' num2str(soffset) '] )']);
        disp([' fit final: amplitude = [' num2str(amplitude) ']     (initial: [' num2str(samplitude) '] )']);
        disp([' fit final: theta     = [' num2str(theta/pi*180) ']     (initial: [' num2str(stheta/pi*180) '] )']);
        disp([' fit final: phi       = [' num2str(phi/pi*180) ']     (initial: [' num2str(sphi/pi*180) '] )']);
        disp([' fit final: alpha     = [' num2str(alpha/pi*180) ']     (initial: [' num2str(salpha/pi*180) '] )']);
    end
    
end


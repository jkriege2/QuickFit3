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



function plot_gaussianprops3D_image( cutpos, plane, offset, amplitude, x0, sigma, theta, phi, alpha, varargin )
%plot_gaussianprops3D_image( cutpos, plane, offset, amplitude, x0, sigma, theta, phi, alpha ) plot the center, axes and lengths into the current plot for the given params of a gaussian
%
%    cutpos specifies the position where the plane is cut through the 3D
%           gaussian
%    plane  'xy', 'yx', 'xz', 'zx', 'yz', 'zy' tells the function which
%           plane to plot

wx=1;
wy=1;
wz=1;
color='white';
optargin = size(varargin,2);
for a=1:2:optargin
    name= varargin{a};
    val= varargin{a+1};

    if (strcmp(name, 'wx') )
        wx=val;
    elseif (strcmp(name, 'wy') )
        wy=val;
    elseif (strcmp(name, 'wxy') )
        wy=val;
        wx=val;
    elseif (strcmp(name, 'wz') )
        wz=val;
    elseif (strcmp(name, 'color') )
        color=val;
    else
        error(['unknown parameter "' name '" to plot_gaussianprops3D_image()']);
    end
end

if strcmp(plane, 'xy')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( x,y,cutpos(3), -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(1), x0(2), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
elseif strcmp(plane, 'yx')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( y,x,cutpos(3), -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(2), x0(1), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
elseif strcmp(plane, 'xz')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( x,cutpos(2),y, -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(1), x0(3), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
elseif strcmp(plane, 'zx')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( y,cutpos(2),x, -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(3), x0(1), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
elseif strcmp(plane, 'yz')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( cutpos(1),x,y, -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(2), x0(3), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
elseif strcmp(plane, 'zy')
    hh=ishold;
    hold on 
    h=ezplot(@(x,y) Gaussian3Drmg( cutpos(1),y,x, -1/(exp(1)^2), 1, x0, sigma, theta, phi, alpha), xlim, ylim);
    set(h, 'Color', color);
    plot(x0(3), x0(2), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    if ~hh
        hold off
    end
end


end


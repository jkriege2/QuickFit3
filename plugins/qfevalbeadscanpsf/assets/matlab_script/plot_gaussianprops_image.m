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

function plot_gaussianprops_image( offset, amplitude, x0, sigma, phi, varargin  )
%plot_gaussianprops_image( offset, amplitude, x0, sigma, phi ) plot the center, axes and lengths into the current plot for the given params of a gaussian

    wx=1;
    wy=1;
    color='white';
    optargin = size(varargin,2);
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'wx') )
            wx=val;
        elseif (strcmp(name, 'wy') )
            wy=val;
        elseif (strcmp(name, 'color') )
            color=val;
        else
            error(['unknown parameter "' name '" to plot_gaussianprops3D_image()']);
        end
    end
    niV=ones(2,2);
    niV(1,2)=tan(phi);
    niV(2,2)=tan(phi+pi/2);
    niV(1,:)=niV(1,:)/norm(niV(1,:));
    niV(2,:)=niV(2,:)/norm(niV(2,:));

    X=[x0(2) x0(2)+niV(1,2)*sigma(1)];
    Y=[x0(1) x0(1)+niV(1,1)*sigma(1)];
    line(X, Y, 'Color', color, 'LineWidth', 2)
    X=[x0(2) x0(2)+niV(2,2)*sigma(2)];
    Y=[x0(1) x0(1)+niV(2,1)*sigma(2)];
    line(X, Y, 'Color', color, 'LineWidth', 2)
    plot(X(1), Y(1), 'Marker', '+', 'MarkerEdgeColor', color, 'MarkerFaceColor', color);
    h=ezplot(@(x,y) Gaussian2Drmg( x,y, -1/(exp(1)^2), 1, x0, sigma, phi), xlim, ylim);
    set(h, 'Color', color);
    h=ezplot(@(x,y) Gaussian2Drmg( x,y, -1/2, 1, x0, sigma, phi), xlim, ylim);
    set(h, 'Color', color);
end


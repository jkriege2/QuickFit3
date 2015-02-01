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

function plot_gaussianprops3D_3D( x0, sigmas, theta, phi, alpha, varargin )
%plot_gaussianprops3D_3D( x0, sigma, theta, phi, alpha ) plots a 3D
%gaussian as FWHM ellipsoid into the current coordinate system. It also
%plots lines along the principal axes of the ellipsoid
%
%    additional parameters in the form plot_gaussianprops3D_3D( ..., 'name', value):
%      'faceColor' [r g b] color of the surface
%      'faceAlpha' [0..1] alpha value of the surface
%      'axisWidth' width of the drawn major axes
%      'xlim' [xmin xmax]: set the xlim of the plot
%      'ylim' [ymin ymax]: set the ylim of the plot
%      'zlim' [zmin zmax]: set the zlim of the plot


    faceColor=[1,0,0];
    axisWidth=2;
    faceAlpha=0.5;
    Nx=100;
    Ny=100;
    Nz=100;
    
    xx=xlim;
    xmin=xx(1);
    xmax=xx(2);
    xx=ylim;
    ymin=xx(1);
    ymax=xx(2);
    xx=zlim;
    zmin=xx(1);
    zmax=xx(2);

    optargin = size(varargin,2);
    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'faceColor') )
            faceColor=val;
        elseif (strcmp(name, 'faceAlpha') )
            faceAlpha=val;
        elseif (strcmp(name, 'axisWidth') )
            axisWidth=val;
        elseif (strcmp(name, 'xlim') )
            xmin=val(1);
            xmax=val(2);
            xlim([xmin xmax]);
        elseif (strcmp(name, 'ylim') )
            ymin=val(1);
            ymax=val(2);
            ylim([ymin ymax]);
        elseif (strcmp(name, 'zlim') )
            zmin=val(1);
            zmax=val(2);
            zlim([zmin zmax]);
        end
    end

    xx=xmin:((xmax-xmin)/Nx):xmax;
    yy=ymin:((ymax-ymin)/Ny):ymax;
    zz=zmin:((zmax-zmin)/Nz):zmax;

    [X Y Z]=meshgrid(xx, yy, zz);
    g=Gaussian3Drmg(X,Y,Z, 0, 1, x0, sigmas, theta, phi, alpha);

    [ er, etheta, ephi ] = sphericalCoordinatesUnitVectors(theta, phi);
    M=directionAngleRotationMatrix(er, alpha);
    rtheta=M*etheta';
    rphi=M*ephi';
    hh=ishold;

    hiso = patch(isosurface(X,Y,Z,g,0.5), 'FaceColor',faceColor, 'EdgeColor','none', 'FaceAlpha', faceAlpha);
    hold on;
    daspect([1 1 1]);
    lightangle(305,30); 
    lighting gouraud; %set(gcf,'Renderer','OpenGL'); %'zbuffer')
    isonormals(g,hiso)
    set(hiso,'SpecularColorReflectance',0,'SpecularExponent',50, 'FaceAlpha', faceAlpha)


    X=[ x0(1) x0(1)+er(1)*sigmas(1)];
    Y=[ x0(2) x0(2)+er(2)*sigmas(1)];
    Z=[ x0(3) x0(3)+er(3)*sigmas(1)];
    line(X,Y,Z, 'Color', 'r', 'LineWidth', axisWidth);
    X=[ x0(1) x0(1)+rtheta(1)*sigmas(2)];
    Y=[ x0(2) x0(2)+rtheta(2)*sigmas(2)];
    Z=[ x0(3) x0(3)+rtheta(3)*sigmas(2)];
    line(X,Y,Z, 'Color', 'g', 'LineWidth', axisWidth);
    X=[ x0(1) x0(1)+rphi(1)*sigmas(3)];
    Y=[ x0(2) x0(2)+rphi(2)*sigmas(3)];
    Z=[ x0(3) x0(3)+rphi(3)*sigmas(3)];
    line(X,Y,Z, 'Color', 'b', 'LineWidth', axisWidth);


    if ~hh
        hold off
    end
end


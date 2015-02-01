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

function result = Gaussian3Drmg(X,Y,Z, offset, amplitude, x0, sigma, varargin)
%Gaussian3Drmg(X,Y,Z, offset, amplitude, x0, sigma, theta, phi, alpha) or 
%Gaussian3Drmg(X,Y,Z, offset, amplitude, x0, sigma, direction, alpha)
%evaluates a 3D gaussian function for the given parameters:
%
%   g(x,y,z)=offset+amplitude*exp(-2*((x-x0)*er)^2/sigma(1)^2 - 2*((x-x0)*etheta)^2/sigma(1)^2 - 2*((x-x0)*ephi)^2/sigma(1)^2)
%
%   X,Y,Z are the coordinates (may be created by meshgrid) of the gaussian
%   offset is a DC offset added to the output
%   amplitude is the amplitude of the gaussian, i.e. g(0,0,0)-offset
%   x0 is the center position as [x y z]
%   sigma are the widths of the gaussian along the three principal axes.
%         the widths are given as 1/e² half-widths.
%   direction (or theta, phi) give the direction of the longest principal
%                             axis in cartesian (->direction) or spherical
%                             (->theta, phi) coordinates. The gaussian will
%                             have a 1/e^2 halfwidth of sigma(1) along
%                             this axis (i.e. the first spherical unit vector 
%                             er. The second sigma(2) will be along the 
%                             second spherical unit vector etheta  and the
%                             third sigma(3) along the third spherical  unit
%                             vector ephi.
%   alpha rotation angle of the spherical unit vectors etheta and ephi
%         around er or direction respectively.
%
% So this function parametrizes the 3D gaussian such that the center and
% the main direction are given. The other two directions are constructed
% perpendicular to this main direction and may be rotated by an angle
% alpha. 

    error(nargchk(9, 10, nargin));
    noptarg=size(varargin,2);

    theta=0;
    phi=0;
    alpha=0;

    if (noptarg==3)
        theta=varargin{1};
        phi=varargin{2};
        alpha=varargin{3};
    elseif noptarg==2
        [theta, phi] = directionToSphericalAngles(varargin{1});
        alpha=varargin{2};
    end
    
    % calculate the three principal axes o the gaussian
    [ er, etheta, ephi ] = sphericalCoordinatesUnitVectors(theta, phi);
    M=directionAngleRotationMatrix(er, alpha);
    e1=er;
    e2=M*etheta';
    e3=M*ephi';
    
    % move to center position
    XX=X-x0(1);
    YY=Y-x0(2);
    ZZ=Z-x0(3);
    
    % project onto the three axes of the gaussian
    EE1=XX*e1(1)+YY*e1(2)+ZZ*e1(3);
    EE2=XX*e2(1)+YY*e2(2)+ZZ*e2(3);
    EE3=XX*e3(1)+YY*e3(2)+ZZ*e3(3);
    
    sigmas=sigma;
    result=offset+amplitude.*exp(-2*(EE1).^2/sigmas(1).^2 - 2*(EE2).^2/sigmas(2).^2 - 2*(EE3).^2/sigmas(3).^2);
    
end


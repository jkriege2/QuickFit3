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

function [ M ] = directionAngleRotationMatrix( direction, angle )
%directionAngleRotationMatrix( direction, angle ) returns a rotation matrix
%around the axis defined by the 3-component vector direction by an angle
%angle.
%   see http://en.wikipedia.org/wiki/Rotation_matrix
ct=cos(angle);
st=sin(angle);
n=direction/norm(direction);
nx=n(1);
ny=n(2);
nz=n(3);

M=[ct+nx^2*(1-ct)      nx*ny*(1-ct)-nz*st   nx*nz*(1-ct)+ny*st; ...
   nx*ny*(1-ct)+nz*st  ct+ny^2*(1-ct)       ny*nz*(1-ct)-nx*st; ...
   nz*nx*(1-ct)-ny*st  nz*ny*(1-ct)+nx*st   ct+nz^2*(1-ct)];

end


function [ er, etheta, ephi ] = sphericalCoordinatesUnitVectors( theta, phi )
%[ er, etheta, ephi ] = sphericalCoordinatesUnitVectors( theta, phi )
%returns the three unit vectors of 3D spherical coordinates, i.e. one
% vectors er in radial direction and two etheta and ephi perpendicular to
% each other and er.
%   the angles 0<theta<pi and 0<phi<2pi are given in radians and describe a
%   direction in 3D space. the vector er points in that direction!
%   see e.g http://de.wikipedia.org/wiki/Datei:Kugelkoordinaten_svg.png for
%   an image of this situation!
%   All three vectors are normalized!

er     = [ sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)  ];
etheta = [ cos(theta)*cos(phi), cos(theta)*sin(phi), -sin(theta) ];
ephi   = [ -sin(phi)          , cos(phi)           , 0           ];

end


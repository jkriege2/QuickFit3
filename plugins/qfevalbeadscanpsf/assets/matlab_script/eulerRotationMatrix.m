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

function [ M ] = eulerRotationMatrix( alpha, beta, gamma )
%eulerRotationMatrix( alpha, beta, gamma ) returns the eulerien rotation
%matrix, as parametrized by the angles alpha, beta and gamma given in the
%x-convention. (see http://de.wikipedia.org/wiki/Eulersche_Winkel)

    % Euler rotation matrices in "x-convention" (see http://de.wikipedia.org/wiki/Eulersche_Winkel)
    M1=[cos(alpha) -sin(alpha) 0; sin(alpha) cos(alpha) 0; 0 0 1];
    M2=[1 0 0 ; 0 cos(beta) -sin(beta); 0 sin(beta) cos(beta) ];
    M3=[cos(gamma) -sin(gamma) 0; sin(gamma) cos(gamma) 0; 0 0 1];
    M=M1*M2*M3;

end


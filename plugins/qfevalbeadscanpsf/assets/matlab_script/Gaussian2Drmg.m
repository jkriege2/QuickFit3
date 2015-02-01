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

function result = Gaussian2Drmg(X,Y, offset, amplitude, x0, sigma, phi)

    XR= X*cos(phi) - Y*sin(phi);
    YR= X*sin(phi) + Y*cos(phi);
    x0r = x0(2)*cos(phi) - x0(1)*sin(phi);
    y0r = x0(2)*sin(phi) + x0(1)*cos(phi);

    result=offset+amplitude.*exp(-2*(XR-x0r).^2/sigma(2).^2 - 2*(YR-y0r).^2/sigma(1).^2);
    
end


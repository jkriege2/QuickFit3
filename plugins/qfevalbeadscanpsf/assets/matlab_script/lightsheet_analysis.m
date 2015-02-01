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

clear

directory='.\2012_06_13\';
filename='lightsheet_zstack_000.lightpath1.tif';
readmode='read_one_file'; % 'read_one_file'  or  'read_file_sequence'

result_basename=[directory filename];
mode='fitall'; % 'fitavg'  or  'fitall';
fitallmode='avg';  % 'single'  or  'avg'
initparammode='takedefault'; %  'takedefault'  or  'takelast'
imagerange=1:100;
pixsize=[0.4 0.4 1];
fitallsteps=32;

expectedWidthRange=[1 3];


if (strcmp(readmode, 'read_one_file'))
    [f,t]=load_tiff_sequence(filename, directory);%, directory);
elseif (strcmp(readmode, 'read_file_sequence'))
    % load image series
    %
    % where mNN.tif are images for x=-NN and pNN.tif is for x=NN
    % these images are combined into one array and then a gaussian is fittet to
    % each row. A figure shows the average+/-stddev of the center and the 1/e²
    % width of the gaussian

    [fm,tm]=load_tiff_sequence([directory filename 'm*.tif'], directory);
    [fp,tp]=load_tiff_sequence([directory filename 'p*.tif'], directory);
    [wm hm Nm]=size(fm);
    [wp hp Np]=size(fp);
    f=zeros(wm,hm,Nm+Np);
    for idx=1:Nm
       f(:,:,idx)=fm(:,:,idx);
    end
    for idx=(Nm+1):(Nm+Np)
       f(:,:,idx)=fp(:,:,idx-Nm);
    end
    t=[-tm tp];
end


perform_lightsheetanalysis(t, f, result_basename, 'pixsize', pixsize, ...
                           'expectedWidthRange', expectedWidthRange, 'imagerange', imagerange, ...
                           'initparammode', initparammode, 'mode', mode, 'fitallsteps', fitallsteps, 'fitallmode', fitallmode);


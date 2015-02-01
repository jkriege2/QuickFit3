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

function [f, t]=load_tiff_sequence(filenam, valuesp)
% load a sequence of image files:
%    load_tiff_sequence(f%02d.tif', 1:5) will load f01.tif, f02.tif,..., f05.tif
%    load_tiff_sequence('f.tif') will load f.tif
%    load_tiff_sequence('f.tif', '.\sub\') will load .\sub\f.tif
%    load_tiff_sequence('f*.tif', '.\sub\') will load .\sub\f01.tif .\sub\f02.tif, ... and return the embedded number in t
% This function returns a 3D array with the images f and a 1D array t with
% image labels (e.g. read from files, or simply the provided values (first
% example)
filename=filenam;
directory='';


f=[];
t=[];


mode=0; % load from multiple files
if nargin==1
    values=0;
    mode=1; % load from one file
else
    values=valuesp;
    if ischar(values)
        directory=valuesp;
        values=0;
        mode=1;
    end
end

disp(['filename=' filename '  dir=' directory '  mode=' num2str(mode) '  values=' mat2str(values)]);


if mode==0
    N=length(values);
    t=values;
    for k=1:N
        v=values(k);
        fn=sprintf(filename, v);
        if (k==1) 
             inf=imfinfo([directory fn]);
             f=zeros(inf.Width, inf.Height, N);
        end
        disp(['opening file ' [directory fn] ]);
        f(:,:,k)=imread([directory fn]);
    end
elseif mode==1
    files=dir([directory filename]);
    disp(['found ' num2str(length(files)) ' files in ' [directory filename] ]);
    if length(files)>1
        disp(['reading ' directory files(1).name]);
        inf=imfinfo([directory files(1).name]);
        N=length(files);
        f=zeros(inf(1).Width, inf(1).Height, N);
        t=1:N;
        for k=1:N
            [m, tt]=regexp(files(k).name, '\D*([0-9]+)\D.*', 'match', 'tokens');
            if ~isempty(tt)
               
                [v, status]=str2num(tt{1}{1});
                if status
                    t(k)=v;
                end
            end
            img=imread([directory files(k).name])';
            simg=size(img);
            disp(['opening file ' num2str(k) '/' num2str(N) ':  ' [directory files(k).name] ', extracted t=' num2str(t(k)) ', width=' num2str(simg(1)) ', height=' num2str(simg(2))]);
            f(:,:,k)=img(1:inf(1).Width, 1:inf(1).Height);
        end
    elseif length(files)==1
        disp(['reading ' directory files(1).name]);
        inf=imfinfo([directory files(1).name]);
        N=length(inf);
        inf=inf(1);
        disp(['frame size ' num2str(inf.Width) 'x' num2str(inf.Height)]);
        f=zeros(inf.Width, inf.Height, N);
        t=1:N;
        for k=1:N
            f(:,:,k)=imread([directory files(1).name], k)';
        end
    end
    
    disp(['   ' num2str(size(f,3)) ' frames found of size ' num2str(size(f,2)) 'x' num2str(size(f,1))]);
    
end


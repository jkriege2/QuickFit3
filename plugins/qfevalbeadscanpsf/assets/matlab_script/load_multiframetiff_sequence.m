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

function [f, t, imgcnt]=load_multiframetiff_sequence(filenam, directory)
% load a sequence of image files where each file contains several images:
%    load_multiframetiff_sequence('f*.tif', '.\sub\') will load .\sub\f01.tif .\sub\f02.tif, ... and return the embedded number in t
% This function returns a 3D array with the images f and a 1D array t with
% image set labels (e.g. read from files, or simply the provided values (first
% example). The third result imgcnt array contains the number of images in each
% file
filename=filenam;


f=[];
t=[];
imgcnt=[];




files=dir([directory filename]);
disp(['found ' num2str(length(files)) ' files in ' [directory filename] ]);

inf=imfinfo([directory files(1).name]);
N=length(files);
t=1:N;
Nimgs=0;
for k=1:N
    [m, tt]=regexp(files(k).name, '\D*([0-9]+)\D.*', 'match', 'tokens');
    if ~isempty(tt)
        [v, status]=str2num(tt{1}{1});
        if status
            t(k)=v;
        end
    end
    inf=imfinfo([directory files(k).name]);
    Nimgs=Nimgs+length(inf);
    imgcnt(k)=length(inf);
end

f=zeros(inf(1).Width, inf(1).Height, Nimgs);

nimg=1;
for k=1:N
    linf=imfinfo([directory files(k).name]);

    for tt=1:length(linf)
        img=imread([directory files(k).name], tt)';
        simg=size(img);
        f(:,:,nimg)=img(1:inf(1).Width, 1:inf(1).Height);
        if tt==1
            disp(['opening file ' num2str(nimg) '/' num2str(Nimgs) ':  ' [directory files(k).name] ', extracted t=' num2str(t(k)) ', width=' num2str(simg(1)) ', height=' num2str(simg(2))]);
        end
        nimg=nimg+1;
    end
    
end

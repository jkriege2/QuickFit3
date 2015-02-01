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

function psf_positions = findBeads3D(data, beads_per_image, stepsize, min_dist, max_size, varargin)
% finds beads in a 3D image stack (greyscale) with pixel accuracy
%
% DESCRIPTION:
% - First a LEVEL for the segmentation is estimated as:
%       LEVEL = quantile(ZSTACK, 1 - BEADS_PER_IMAGE/(FRAME_WIDTH*FRAME_HEIGHT) )
%   So we expect to find BEADS_PER_IMAGE beads in each plane and therefore
%   the level is selected so on average the BEADS_PER_IMAGE brightest
%   pixels are used. 
%   So the parameter BEADS_PER_IMAGE is more the average number of pixels
%   in each image that MAY be part of a bead, so it is in the best case
%   only the maximum number of beads per image.
% - The algorithm steps through the z-stack in steps of length STEPSIZE. Then
%   for each of these planes it performs the following steps:
%     1. The plane is segmented according to a simple level segmentation,
%        i.e. only those pixels may be part of a bead where the pixel
%        greyvalue is larger than the LEVEL.
%     2. calculate the distance matrix, i.e. the matrix where the entry
%        d(i1, i2) is the distance between the i1-th and i2th pixel in the
%        list created in step 1 (euclidean distance).
%     3. list the coordinates of all pixels with greyvalue>LEVEL and then
%        go though this list (initially ignore NO pixels and perform the
%        following steps only for non-ignored pixels):
%          3.1 find all pixels within MIN_DIST of the current pixel that
%              are not ignore
%          3.2 if there is only one pixel: add this pixel as a bead
%          3.3 if there are more pixels, add the pixel with the highest
%              intensity and add the others to the ignore list. Do not add
%              a pixel if there are more that pi*(MAX_SIZE/2)^2 pixels in
%              its vicinity. This last condition is there to exclude really
%              large blobs or groups of very many nearby beads. 
% - Finally for each found bead, the z-coordinate is refined, by searching
%   in the range [z-STEPSIZE/2 ... z+STEPSIZE/2] for the brightest pixel (x
%   and y are kept fixed.
%
% PARAMETERS:
%    data              a 3D image stack (w*h*N) of N frames, each w pixels
%                      wide and h pixels high.
%    beads_per_image   average number of pixels in each plane that may be
%                      part of a bead after level-segmentation, i.e. also 
%                      the maximum number of beads in each frame. The
%                      segmentation level is estimated from this
%    stepsize          the stepsize the algorithm uses to step through the
%                      image stack, starting at plane 1
%    min_dist          minimum distance in pixels between two adjacent beads
%    max_size          maximum number of pixels belonging to a bead, or near 
%                      a bead after segmentation
%    cutoutZ           don't use beads found in the z-ranges in this cell
%                      array
%    normalizeintensity if set true, the average intensity of each image is
%                      normalized to the complete stacks' average intensity
%                      before searching for beads
% ADDITIONAL SWITCHES/ARGUMENTS:
%    'plot'            output plots during processing (for debugging)
%    'verbose'         output additional debugging information during
%                      processing (text messages)

doPlot=false;
verbose=false;
optargin = size(varargin,2);
normalizeintensity=false;
cutoutZ={};
if (optargin>0)
    oidx=1;
    while (oidx<=optargin)
%        if (ischar(varargin(oidx)))
%            varargin(oidx)
            if (strcmp(varargin(oidx), 'plot'))
                doPlot=true;
            elseif (strcmp(varargin(oidx), 'verbose'))
                verbose=true;
            elseif (strcmp(varargin(oidx), 'normalizeintensity'))
                normalizeintensity=true;
            elseif (strcmp(varargin(oidx), 'cutoutZ'))
                oidx=oidx+1;
                cutoutZ=varargin{oidx}
            end
%        end
        oidx=oidx+1;
    end
end
psf_positions=[];
[w h N]=size(data);

dataNotNan=~isnan(data(:));

pp=beads_per_image/(w*h);
level=quantile(data(dataNotNan), 1-pp);

SE = strel('arbitrary', ones(6,6));


avgIntensity=mean(data(dataNotNan));

if normalizeintensity
    for plane=1:N
        d=data(:,:,plane);
        dnn=~isnan(d(:));
        data(:,:,plane)=d/mean(d(dnn))*avgIntensity;
    end
end

psfpos_idx=1;
for plane=1:stepsize:N
%test_plane=200;
%for plane=test_plane:test_plane
    if verbose
        disp(['searching in plane ' num2str(plane) ':']);
    end
    pos=[];
    img=data(:,:,plane);
    img_level=(~isnan(img))&(img>level);
    
    [r c]=find(img_level==1);

    
    dist=zeros(length(r), length(r));
    for rr=1:length(r)
        dist(rr,1:length(r))=sqrt((r(rr)-r(:)).^2+(c(rr)-c(:)).^2);
    end
    dist_level=dist<min_dist;
    
    pcnt=1;
    ignore=false(1,length(r));
    pos(1:length(r),1:3)=0;
    for ir=1:length(r)        
        if (~ignore(ir))
            idxs=dist(ir,:)<min_dist;
            idxsum=sum(idxs);
            if (idxsum>1 && idxsum<(pi*max_size.^2/4))
                img_indexes=sub2ind(size(img), r(idxs), c(idxs));
                d=img(img_indexes);
                [C,I]=max(d);
                temp_idxs=find(idxs==1);
                
                %[find(idxs); r(idxs)'; c(idxs)']'
                ignore(idxs)=true;
                ignore(ir)=true;
                pos(pcnt,1:3)=[r(temp_idxs(I(1))) c(temp_idxs(I(1))) plane];
                pcnt=pcnt+1;
                if verbose
                    disp(['   ' num2str(ir) ': excluding ' num2str(idxsum) ' beads ... ' num2str(sum(~ignore)) ' left']);
                    disp(['   ' num2str(ir) ': adding no.' num2str(pcnt-1) ' [' num2str(r(I(1))) ', ' num2str(c(I(1))) ' ]']);
                end
            elseif (idxsum==1)
               pos(pcnt,1:3)=[r(ir) c(ir) plane ];
               pcnt=pcnt+1;
                if verbose
                   disp(['   ' num2str(ir) ': adding no.' num2str(pcnt-1) ' [' num2str(r(ir)) ', ' num2str(c(ir)) ' ]']);
                end
               ignore(ir)=true;
            end
        end
    end
    
    pcnt=pcnt-1;
    
    if (pcnt>1)
        pos=pos(1:pcnt,:);
        psf_positions(psfpos_idx:(psfpos_idx+pcnt-1),1:3)=pos;
        psfpos_idx=psfpos_idx+pcnt;
    else
        if verbose
            disp(['   no beads found!']);
        end
    end
    
    %idx=sum(dist_level,1)>length(r)*min_dist;
    %idx=sum(dist_level,1)<3;
    
    %pos(:,1)=r(idx);
    %pos(:,2)=c(idx);
    
    [Np, d]=size(pos);

    if verbose
        disp(['   distance sum level: ' num2str(length(r)*min_dist) ]);
        disp(['   found ' num2str(Np) ' beads']);
    end
        
    s=sum(dist_level,1);
    if (doPlot && ~isempty(s))    
        figure(1);
        subplot(1,4,3);
        imagesc(dist);
        subplot(1,4,4);
        plot(s,0.5+(length(s):(-1):1));
        ylim([1 length(s)+1])
        subplot(1,4,1);
        imagesc(data(:,:,plane));
        hold on
        if (Np>0)
            plot(pos(:,2), pos(:,1), 'r+');
        end
        hold off;

        subplot(1,4,2);
        imagesc(img_level);
        hold on

        if (Np>0)
            plot(pos(:,2), pos(:,1), '+g');
        end
        hold off;
        drawnow;
    end
end


if verbose
    disp(['refining z-position of ' num2str(length(psf_positions)) ' beads:']);
end

for idx=1:size(psf_positions,1)
    x=psf_positions(idx,1);
    y=psf_positions(idx,2);
    z=psf_positions(idx,3);
    zz=max(1,(z-round(stepsize/2))):min((z+round(stepsize/2)-1), N);
    d=data(x,y,zz);
    d(d==NaN)=0;
    [C I]=max(d);
    if (~isempty(I))
        if (z~=zz(I(1)))
            psf_positions(idx,3)=zz(I(1));
            if verbose
                disp(['  refining [ ' num2str(x) ', ' num2str(y) ', ' num2str(z) ' ] to [ '  num2str(x) ', ' num2str(y) ', ' num2str(zz(I(1))) ' ]']);
            end
        end
    end
end

if ~isempty(cutoutZ)    
    if verbose
        disp(['cleaning z-position of ' num2str(length(psf_positions)) ' beads:']);
    end
    
    use=true(size(psf_positions,1),1);
    for cnt=1:length(cutoutZ)
        range=cutoutZ{cnt};
        use((psf_positions(:,3)>=range(1)) & (psf_positions(:,3)<=range(2)))=false;
    end
    psf_positions=psf_positions(use,:);
end

if (sum(~dataNotNan(:))>0)
    if verbose
        disp(['removing beads near NaN pixels ...']);
    end
    dd=[1  0  0;
        0  1  0;
        0  0  1;
       -1  0  0;
        0 -1  0;
        0  0 -1;
        1  1  0;
        0  1  1;
        1  0  1;
       -1  1  0;
        1 -1  0;
        0 -1  1;
        0  1 -1;
        1  0 -1;
       -1  0 -1;
        0 -1 -1;
       -1 -1  0;
       -1  0  1;
        1  1  1;
       -1  1  1;
        1 -1  1;
        1  1 -1;
        1 -1 -1;
       -1  1 -1;
       -1 -1  1;
       -1 -1 -1];
    
    use=true(size(psf_positions,1),1);
    for cnt=1:size(psf_positions,1)
        tst=repmat(psf_positions(cnt,:),26,1)+dd;
        tstOK=tst(:,1)>0 & tst(:,1)<=w & tst(:,2)>0 & tst(:,2)<=h & tst(:,3)>0 & tst(:,3)<=N;
        tst=tst(tstOK,:);
        tstidx=sub2ind(size(data), tst(:,1), tst(:,2), tst(:,3));
        use(cnt)=(sum(isnan(data(tstidx)))==0);
    end
    psf_positions=psf_positions(use,:);

end

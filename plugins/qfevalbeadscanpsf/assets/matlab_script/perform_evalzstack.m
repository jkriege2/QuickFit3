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

function [ output_args ] = perform_evalzstack( data, psf_positions, output_prefix, varargin )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    optargin = size(varargin,2);

    datarange=[];
    deltaz = 100;         % longitudinal pixel-size in nanometers
    deltaxy = 412;       % lateral pixelsize in nanometers
    cutregion_xy = 3;     % size of the region to cut laterally
    cutregion_z = 25;     % size of the region to cut longitudinally
    max_intensity = 0.95;
    maxfunevals=3000;
    maxiter=1000;
    showsinglebeadplots=true;
    fit3Dgaussian=true;
    showeveryNthSinglebeadPlot=1;
    
    maxxwidth=2500;
    maxywidth=2500;
    maxzwidth=2500;
    histgammarange=[0.1 6];
    histgammabinwidth=0.1;
    histwidthxyrange=[400 1500];
    histwidthzrange=[400 1500];
    histwidthxybinwidth=10;
    histwidthzbinwidth=10;
    
    fitmodel_xy='gaussian';
    fitmodel_z='gaussian';
    fitxy_pixelwidth=deltaxy;
    fitz_pixelwidth=deltaz;
    
    hasfitxy_pixelwidth=false;
    hasfitz_pixelwidth=false;
    fitxzcolumns=false;

    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'datarange') )
            datarange=val;
        elseif (strcmp(name, 'histgammarange') )
            histgammarange=val;
        elseif (strcmp(name, 'histwidthxyrange') )
            histwidthxyrange=val;
        elseif (strcmp(name, 'histwidthzrange') )
            histwidthzrange=val;
        elseif (strcmp(name, 'histgammabinwidth') )
            histgammabinwidth=val;
        elseif (strcmp(name, 'histwidthxybinwidth') )
            histwidthxybinwidth=val;
        elseif (strcmp(name, 'histwidthzbinwidth') )
            histwidthzbinwidth=val;
        elseif (strcmp(name, 'deltaz') )
            deltaz=val;
        elseif (strcmp(name, 'deltaxy') )
            deltaxy=val;
        elseif (strcmp(name, 'cutregion_xy') )
            cutregion_xy=val;
        elseif (strcmp(name, 'cutregion_z') )
            cutregion_z=val;
        elseif (strcmp(name, 'max_intensity') )
            max_intensity=val;
        elseif (strcmp(name, 'maxxwidth') )
            maxxwidth=val;
        elseif (strcmp(name, 'maxywidth') )
            maxywidth=val;
        elseif (strcmp(name, 'maxzwidth') )
            maxzwidth=val;
        elseif (strcmp(name, 'maxfunevals') )
            maxfunevals=val;
        elseif (strcmp(name, 'maxiter') )
            maxiter=val;  
        elseif (strcmp(name, 'fitmodel_xy') )
            fitmodel_xy=val;  
        elseif (strcmp(name, 'fitmodel_z') )
            fitmodel_z=val;  
        elseif (strcmp(name, 'fitxy_pixelwidth') )
            hasfitxy_pixelwidth=true;
            fitxy_pixelwidth=val;  
        elseif (strcmp(name, 'fitz_pixelwidth') )
            hasfitz_pixelwidth=true;  
            fitz_pixelwidth=val;  
        elseif (strcmp(name, 'showsinglebeadplots') )
            showsinglebeadplots=val;
        elseif (strcmp(name, 'showeveryNthSinglebeadPlot') )
            showeveryNthSinglebeadPlot=val;
        elseif (strcmp(name, 'fit3Dgaussian') )
            fit3Dgaussian=val;
        elseif (strcmp(name, 'fitxzcolumns') )
            fitxzcolumns=val;
        else
            error(['unknown parameter "' name '" to perform_evalzstack()']);
        end
    end

    
    if length(histgammarange)==2
        histgammaX=histgammarange(1):histgammabinwidth:histgammarange(2);
    elseif length(histgammarange)>2
        histgammaX=histgammarange;
    else
        error('histgammarange has to have at least 2 entries');
    end
    
    if length(histwidthxyrange)==2
        histwidthxyX=histwidthxyrange(1):histwidthxybinwidth:histwidthxyrange(2);
    elseif length(histwidthxyrange)>2
        histwidthxyX=histwidthxyrange;
    else
        error('histwidthxyrange has to have at least 2 entries');
    end

    if length(histwidthzrange)==2
        histwidthzX=histwidthzrange(1):histwidthzbinwidth:histwidthzrange(2);
    elseif length(histwidthzrange)>2
        histwidthzX=histwidthzrange;
    else
        error('histwidthzrange has to have at least 2 entries');
    end
    

    
    if ~hasfitxy_pixelwidth
        fitxy_pixelwidth=deltaxy;
    end
    if ~hasfitz_pixelwidth
        fitz_pixelwidth=deltaz;
    end
    %data=frames;

    [w,h,N]=size(data);
    if (isempty(datarange)) 
        datarange=1:N;
    end

    data=data(:,:,datarange);

    options = optimset('lsqcurvefit');
    options = optimset(options, 'MaxFunEvals',maxfunevals);
    options = optimset(options, 'MaxIter',maxiter);
    options = optimset(options, 'Display','off');

    
    temp=mean(data,3);
    qi=quantile(temp(:), max_intensity);
    ds=size(data);
    %a=temp<qi;
    for cnt=1:ds(3)
        data(:,:,cnt)=data(:,:,cnt).*double(qi);
    end


    datamin=min(min(min(data)));
    datamax=max(max(max(data)));
    
    psf_positions=psf_positions+1;
    spsf=size(psf_positions);
    
    dataout.datarange=datarange;
    dataout.deltaz = deltaz;
    dataout.deltaxy = deltaxy;
    dataout.cutregion_xy = cutregion_xy;
    dataout.cutregion_z = cutregion_z;
    dataout.max_intensity = max_intensity;
    dataout.maxfunevals=maxfunevals;
    dataout.maxiter=maxiter;
    
    dataout.histgammarange=histgammarange;
    dataout.histgammabinwidth=histgammabinwidth;
    dataout.histwidthxyrange=histwidthxyrange;
    dataout.histwidthzrange=histwidthzrange;
    dataout.histwidthxybinwidth=histwidthxybinwidth;
    dataout.histwidthzbinwidth=histwidthzbinwidth;
    
    dataout.maxxwidth=maxxwidth;
    dataout.maxywidth=maxywidth;
    dataout.maxzwidth=maxzwidth;
    dataout.datamin=datamin;
    dataout.datamax=datamax;
    dataout.psf_positions=psf_positions;
    dataout.w=w;
    dataout.h=h;
    dataout.N=N;
    dataout.avgIntensity=mean(data(:));
    
    
    cnt=1;
    for cnt1=1:spsf(1)
        x=psf_positions(cnt1,1);
        y=psf_positions(cnt1,2);
        z=psf_positions(cnt1,3);
        %p=psf_positions(cnt1,:);
        datasize=size(data);
        if (y-cutregion_xy>0 && x-cutregion_xy>0 && z-cutregion_z>0 && x+cutregion_xy<=datasize(1) && y+cutregion_xy<=datasize(2) && z+cutregion_z<=datasize(3))
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % cut region around bead from the raw data array
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            psf=double(data((x-cutregion_xy):(x+cutregion_xy), (y-cutregion_xy):(y+cutregion_xy), (z-cutregion_z):(z+cutregion_z)));
            psf_min=min(psf(~isnan(psf(:))));
            psf_max=max(psf(~isnan(psf(:))));

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % find bead by finding the brightest pixel in the psf
            % subregion!
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            temp=find(psf==psf_max);
            if (length(temp)>=1)
                [psf_maxpos(1) psf_maxpos(2) psf_maxpos(3)]=ind2sub([2*cutregion_xy+1, 2*cutregion_xy+1, 2*cutregion_z+1], temp(1+floor(length(temp)/2)));
            else
                psf_maxpos(1)=cutregion_xy+1;
                psf_maxpos(2)=cutregion_xy+1;
                psf_maxpos(3)=cutregion_z+1;
            end

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % make a 1D cut through the psf region, the z-coordinate axis
            % and going through the brightest pixel.
            % This will be used for the line-fit and then to refine the 
            % z-position of the bead!!
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            cutz=squeeze(psf(psf_maxpos(1),psf_maxpos(2),:));
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % z-fit and xy-fit model
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            if strcmp(fitmodel_z, 'pixel')
                ffz=@(p,x) fit_rectconvgauss1D([p(1) p(2) p(3) p(4) fitz_pixelwidth],x);
            else
                ffz=@(p,x) fit_gauss1D([p(1) p(2) p(3) p(4)],x);
            end
            if strcmp(fitmodel_xy, 'pixel')
                ffxy=@(p,x) fit_rectconvgauss1D([p(1) p(2) p(3) p(4) fitxy_pixelwidth],x);
            else
                ffxy=@(p,x) fit_gauss1D([p(1) p(2) p(3) p(4)],x);
            end

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Fit along z-axis through max. intensity pixel
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            cutZaxis=((1:length(cutz))'-1).*deltaz;
            gparam0=[psf_max-psf_min max(cutZaxis)/2 3*deltaz psf_min];
            temp=find(cutz==max(cutz));
            if (length(temp)>=1)
                gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
            end
            zAxisIsNNan=~isnan(cutz);
            gparamz(cnt,:) = lsqcurvefit(ffz, gparam0, cutZaxis(zAxisIsNNan), cutz(zAxisIsNNan),[],[], options);
            gabspos(cnt,3)=gparamz(cnt,3)+(z-1-cutregion_z)*deltaz;
            %gparamz(cnt,3)=min(maxzwidth, gparamz(cnt,3));
            beadout.cutz_y=cutz;
            beadout.cutz_x=cutZaxis;
            beadout.cutz_fitresults=gparamz(cnt,:);
            beadout.cutz_absbeadpos=gabspos(cnt,3);

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % from the last fit results we can refine the z-position of the
            % bead! This z-position (rounded to integers) is used for the
            % x- and y-cut and fit.
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            zmaxpos=uint16(round(gparamz(cnt,2)/deltaz)+1);
            beadout.zmaxpos=zmaxpos;            

            
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Fit along x-axis through max. intensity pixel
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            spsf=size(psf);
            cutx=squeeze(psf(:,max(1,min(spsf(2),psf_maxpos(2))),max(1,min(spsf(3),zmaxpos))));
            cutXaxis=((1:length(cutx))'-1).*deltaxy;
            gparam0=[psf_max-psf_min max(cutXaxis)/2 deltaxy psf_min];
            temp=find(cutx==max(cutx));
            if (length(temp)>=1)
                gparam0(2)=cutXaxis(temp(1+floor(length(temp)/2)));
            end
            xAxisIsNNan=~isnan(cutx);
            gparamx(cnt,:)= lsqcurvefit(ffxy, gparam0, cutXaxis(xAxisIsNNan), cutx(xAxisIsNNan),[],[],options);
            gabspos(cnt,1)=gparamx(cnt,2)+(x-1-cutregion_xy)*deltaxy;
            %gparamx(cnt,3)=min(maxxwidth, gparamx(cnt,3));
            beadout.cutx_y=cutx;
            beadout.cutx_x=cutXaxis;
            beadout.cutx_fitresults=gparamx(cnt,:);
            beadout.cutx_absbeadpos=gabspos(cnt,1);

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Fit along y-axis through max. intensity pixel
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            cuty=squeeze(psf(max(1,min(spsf(1),psf_maxpos(1))),:,max(1,min(spsf(3),zmaxpos))));
            cutYaxis=((1:length(cuty))-1).*deltaxy;
            gparam0=[psf_max-psf_min max(cutYaxis)/2 deltaxy psf_min];
            temp=find(cuty==max(cuty));
            if (length(temp)>=1)
                gparam0(2)=cutYaxis(temp(1+floor(length(temp)/2)));
            end
            yAxisIsNNan=~isnan(cuty);
            gparamy(cnt,:) = lsqcurvefit(ffxy, gparam0, cutYaxis(yAxisIsNNan), cuty(yAxisIsNNan),[],[],options);
            gabspos(cnt,2)=gparamy(cnt,2)+(y-1-cutregion_xy)*deltaxy;
            %gparamy(cnt,3)=min(maxywidth, gparamy(cnt,3));
            beadout.cuty_y=cuty;
            beadout.cuty_x=cutYaxis;
            beadout.cuty_fitresults=gparamy(cnt,:);
            beadout.cuty_absbeadpos=gabspos(cnt,2);
            
            
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % Fit along x-axis through additional points on z-axis
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            beadout.fitxzcolumns=fitxzcolumns;
            if (fitxzcolumns)
                widpixs=round(beadout.cutz_fitresults(3)/deltaz);
                zstart=max(1,zmaxpos-2*widpixs);
                zend=min(spsf(3),zmaxpos+2*widpixs);
                
                beadout.xzcolumns.zstart=zstart;
                beadout.xzcolumns.zend=zend;
                
                zcnt=1;
                zgparamx=[];
                zgparamy=[];
                zzvec=zstart:zend;
                if (length(zzvec)>3)
                    zzvec_small=find(zzvec>zmaxpos-widpixs & zzvec<zmaxpos+widpixs);
                    for zz=zzvec
                        % Fit along x-axis through max. intensity pixel
                        zcutx=squeeze(psf(:,max(1,min(spsf(2),psf_maxpos(2))),max(1,min(spsf(3),zz))));
                        zcutXaxis=((1:length(cutx))'-1).*deltaxy;
                        zgparam0=[psf_max-psf_min max(zcutXaxis)/2 deltaxy psf_min];
                        temp=find(zcutx==max(zcutx));
                        if (length(temp)>=1)
                            zgparam0(2)=zcutXaxis(temp(1+floor(length(temp)/2)));
                        end
                        zxAxisIsNNan=~isnan(zcutx);
                        zgparamx(zcnt,:)= lsqcurvefit(ffxy, zgparam0, zcutXaxis(zxAxisIsNNan), zcutx(zxAxisIsNNan),[],[],options);
                        zgabspos(zcnt,1)=zgparamx(zcnt,2)+(x-1-cutregion_xy)*deltaxy;
                        beadout.xzcolumns.cutx_y(zcnt,:)=zcutx;
                        beadout.xzcolumns.cutx_x(zcnt,:)=zcutXaxis;

                        % Fit along y-axis through max. intensity pixel
                        zcuty=squeeze(psf(max(1,min(spsf(1),psf_maxpos(1))),:,max(1,min(spsf(3),zz))));
                        zcutYaxis=((1:length(zcuty))-1).*deltaxy;
                        zgparam0=[psf_max-psf_min max(zcutYaxis)/2 deltaxy psf_min];
                        temp=find(zcuty==max(zcuty));
                        if (length(temp)>=1)
                            zgparam0(2)=zcutYaxis(temp(1+floor(length(temp)/2)));
                        end
                        zyAxisIsNNan=~isnan(zcuty);
                        zgparamy(zcnt,:) = lsqcurvefit(ffxy, zgparam0, zcutYaxis(zyAxisIsNNan), zcuty(zyAxisIsNNan),[],[],options);
                        zgabspos(zcnt,2)=zgparamy(zcnt,2)+(y-1-cutregion_xy)*deltaxy;
                        beadout.xzcolumns.cuty_y(zcnt,:)=zcuty;
                        beadout.xzcolumns.cuty_x(zcnt,:)=zcutYaxis;

                        zcnt=zcnt+1;
                    end
                    beadout.xzcolumns.cutx_fitresults=zgparamx;
                    beadout.xzcolumns.cuty_fitresults=zgparamy;

                    % fit gaussian width to fit results
                    zevccoord=double(zzvec-1)*deltaz;
                    xp0=[beadout.cutx_fitresults(3) beadout.cutz_fitresults(3) double(zmaxpos-1)*deltaz];
                    yp0=[beadout.cuty_fitresults(3) beadout.cutz_fitresults(3) double(zmaxpos-1)*deltaz];
                    zevccoord_fine=zevccoord(1):((zevccoord(end)-zevccoord(1))/500):zevccoord(end);
                    %size(xp0)
                    %size(zevccoord)
                    %size(zgparamx(:,3))
                    ffp=@(p,x) fit_gaussianbeam_width(p,x);
                    xp = lsqcurvefit(ffp, xp0, zevccoord(zzvec_small)', zgparamx(zzvec_small,3),[],[],options);
                    yp = lsqcurvefit(ffp, yp0, zevccoord(zzvec_small)', zgparamy(zzvec_small,3),[],[],options);
                    beadout.xzcolumns.gaussianwidthfit_x=xp;
                    beadout.xzcolumns.gaussianwidthfit_y=yp;
                    xzcolumns_gaussianwidthfit_x(cnt,:)=xp;
                    xzcolumns_gaussianwidthfit_y(cnt,:)=yp;


                    if showsinglebeadplots && (mod(cnt-1, showeveryNthSinglebeadPlot)==0)
                        h=figure(4);


                        subplot(2,1,1);
                        plot(zevccoord, zgparamx(:,3), 'r+', ...
                            double(zmaxpos-1)*deltaz, beadout.cutx_fitresults(3), 'b*', ...
                            zevccoord_fine, fit_gaussianbeam_width(xp, zevccoord_fine), 'g-');
                        legend('fit results', 'max. datapoint', ['gaussian beam fit w0=' num2str(xp(1)) 'nm, z0=' num2str(xp(2)) 'nm, pos=' num2str(xp(3)) 'nm']);
                        xlabel('z [nm]');
                        ylabel('PSF width [nm]');
                        title('x-cut fits');
                        subplot(2,1,2);
                        plot(zevccoord, zgparamy(:,3), 'r+', ...
                            double(zmaxpos-1)*deltaz, beadout.cuty_fitresults(3), 'b*', ...
                            zevccoord_fine, fit_gaussianbeam_width(yp, zevccoord_fine), 'g-');
                        legend('fit results', 'max. datapoint', ['gaussian beam fit w0=' num2str(yp(1)) 'nm, z0=' num2str(yp(2)) 'nm, pos=' num2str(yp(3)) 'nm']);
                        xlabel('z [nm]');
                        ylabel('PSF width [nm]');
                        title('y-cut fits');

                        orient(h, 'tall');
                        saveas(h, [output_prefix '.psf' num2str(cnt) '_xzcol.pdf']);
                        saveas(h, [output_prefix '.psf' num2str(cnt) '_xzcol.fig']);
                    end
                end

            end
           
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % 3D gaussian fit
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            if fit3Dgaussian
                fitresults_3D=zeros(11,1);
                [ fitresults_3D(1:3) fitresults_3D(4:6) fitresults_3D(7) fitresults_3D(8) fitresults_3D(9) fitresults_3D(10) fitresults_3D(11) ] = performfit_gaussian3Dr( psf, 'verbose', false, ...
                                     'w', [fitxy_pixelwidth fitxy_pixelwidth fitz_pixelwidth], ...
                                     'init', [ gparamx(cnt,2) gparamy(cnt,2) gparamz(cnt,2) gparamz(cnt,3) gparamx(cnt,3) gparamy(cnt,3) 0 0 0 gparamz(cnt,4) gparamy(cnt,1)] ...
                                     );
                gparam3D(cnt,:)= fitresults_3D;
                gabspos3D(cnt,1:3)=gparam3D(cnt,1:3)+[(x-1-cutregion_xy)*deltaxy (y-1-cutregion_xy)*deltaxy (z-1-cutregion_z)*deltaz];
                beadout.beadpos_fit3D=fitresults_3D(1:3);
                beadout.beadwidths_fit3D=fitresults_3D(4:6);
            end

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % complete data record for the data of this bead
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            beadout.psf=psf;
            beadout.beadpos=[x y z];
            beadout.num_abs=cnt1;
            beadout.num=cnt;
            beadout.psf_min=psf_min;
            beadout.psf_max=psf_max;
            beadout.psf_maxpos=psf_maxpos;
            beadout.cutx=cutx;
            beadout.cuty=cuty;
            beadout.cutz=cutz;
            beadout.deltaxy=deltaxy;
            beadout.deltaz=deltaz;  
            beadout.absbeadpos_fitresult=gabspos(cnt,:);
            if fit3Dgaussian
                beadout.fitresults_3D=fitresults_3D;
            end

            
            
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % create plots for this bead and save them. This also saves the
            % data of this bead as .mat-file!
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            if showsinglebeadplots && (mod(cnt-1, showeveryNthSinglebeadPlot)==0)
                icutx=squeeze(psf(psf_maxpos(1),:,:));
                icuty=squeeze(psf(:,psf_maxpos(2),:));

                h=figure(2);
                colormap('default')
                cla
                set(h, 'PaperOrientation', 'landscape');
                subplot(3,3,1)
                imagesc(icuty, [min(icuty(:)) max(icuty(:))] );
                hold on
                plot_gaussianprops_image( 0, 1, [gparamx(cnt,2)/deltaxy+1 gparamz(cnt,2)/deltaz+1 ], [gparamx(cnt,3)/deltaxy gparamz(cnt,3)/deltaz], 0, 'color', [0.5 0.5 0.5] );
                hold off
                daspect([deltaxy/deltaz 1 1 ]);
                ylabel('x [pixel]');
                xlabel('z [pixel]');
                title(['x=' num2str(psf_maxpos(1)) ' cut / point ' num2str(cnt) ': (' num2str(x) ', ' num2str(y) ', ' num2str(z) ')']);
                subplot(3,3,2)
                imagesc(icutx, [min(icutx(:)) max(icutx(:))] );
                hold on
                plot_gaussianprops_image( 0, 1, [gparamy(cnt,2)/deltaxy+1 gparamz(cnt,2)/deltaz+1 ], [gparamy(cnt,3)/deltaxy gparamz(cnt,3)/deltaz], 0, 'color', [0.5 0.5 0.5] );
                hold off
                daspect([deltaxy/deltaz 1 1 ]);
                ylabel('y [pixel]');
                xlabel('z [pixel]');
                title(['y=' num2str(psf_maxpos(2)) ' cut']);


                subplot(3,3,4)
                imagesc(log(double(icuty))/log(10), log(double([min(icuty(:)) max(icuty(:))]))/log(10));
                hold on
                plot_gaussianprops_image( 0, 1, [gparamx(cnt,2)/deltaxy+1 gparamz(cnt,2)/deltaz+1 ], [gparamx(cnt,3)/deltaxy gparamz(cnt,3)/deltaz], 0, 'color', [0.5 0.5 0.5] );
                hold off
                daspect([deltaxy/deltaz 1 1 ]);
                ylabel('x [pixel]');
                xlabel('z [pixel]');
                title(['log(y=' num2str(psf_maxpos(2)) ' cut)']);
                subplot(3,3,5)
                imagesc(log(double(icutx))/log(10), log(double([min(icutx(:)) max(icutx(:))]))/log(10) );
                hold on
                plot_gaussianprops_image( 0, 1, [gparamy(cnt,2)/deltaxy+1 gparamz(cnt,2)/deltaz+1 ], [gparamy(cnt,3)/deltaxy gparamz(cnt,3)/deltaz], 0, 'color', [0.5 0.5 0.5] );
                hold off
                daspect([deltaxy/deltaz 1 1 ]);
                ylabel('y [pixel]');
                xlabel('z [pixel]');
                title(['log(x=' num2str(psf_maxpos(1)) ' cut)']);

                subplot(3,3,9)
                cutz_fitZaxis=(0:0.2:length(cutz)).*deltaz;
                plot(cutZaxis, cutz, 'r+', cutz_fitZaxis, ffz(gparamz(cnt,:), cutz_fitZaxis), 'b-');
                text(0.7,0.9,['wz = ' num2str(gparamz(cnt,3)) ' nm' ], 'Units', 'normalized', 'FontSize', 8);
                text(0.7,0.7,['(x,y) = (' num2str(psf_maxpos(1)) ', ' num2str(psf_maxpos(2)) ') px' ], 'Units', 'normalized', 'FontSize', 6);
                xlabel('z [nm]');
                ylabel('intensity [A.U.]');

                subplot(3,3,1)
                rectangle('Position', [0.5 psf_maxpos(1)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
                rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(1)-0.5 1 1], 'EdgeColor', 'white');
                subplot(3,3,2)
                rectangle('Position', [0.5 psf_maxpos(2)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
                rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
                subplot(3,3,4)
                rectangle('Position', [0.5 psf_maxpos(1)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
                rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(1)-0.5 1 1], 'EdgeColor', 'white');
                subplot(3,3,5)
                rectangle('Position', [0.5 psf_maxpos(2)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
                rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');

                subplot(3,3,3)
                spsf8=size(psf);
                cutz=squeeze(psf(:,:,max(1,min(zmaxpos, spsf8(3))))');
                imagesc(cutz, [min(cutz(:)) max(cutz(:))] );
                hold on
                plot_gaussianprops_image( 0, 1, [gparamy(cnt,2)/deltaxy+1 gparamx(cnt,2)/deltaxy+1 ], [gparamy(cnt,3)/deltaxy gparamx(cnt,3)/deltaxy], 0, 'color', [0.5 0.5 0.5] );
                hold off
                rectangle('Position', [ psf_maxpos(1)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
                daspect([1 1 1]);
                ylabel('x [pixel]');
                xlabel('y [pixel]');
                title(['z=' num2str(max(1,min(zmaxpos, spsf8(3)))) ' cut']);

                subplot(3,3,6)
                imagesc(log(double(cutz))/log(10), log(double([min(cutz(:)) max(cutz(:))]))/log(10) );
                hold on
                plot_gaussianprops_image( 0, 1, [gparamy(cnt,2)/deltaxy+1 gparamx(cnt,2)/deltaxy+1 ], [gparamy(cnt,3)/deltaxy gparamx(cnt,3)/deltaxy], 0, 'color', [0.5 0.5 0.5] );
                hold off
                rectangle('Position', [ psf_maxpos(1)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
                daspect([1 1 1]);
                ylabel('x [pixel]');
                xlabel('y [pixel]');
                title(['log(z=' num2str(max(1,min(zmaxpos, spsf8(3)))) ' cut)']);

                subplot(3,3,7)
                cutx_fitXaxis=(0:0.2:length(cutx)).*deltaxy;
                cutx_fit=ffxy(gparamx(cnt,:), cutx_fitXaxis);
                plot(cutXaxis, cutx, 'r+', cutx_fitXaxis, cutx_fit, 'b-');
                xlabel('x [nm]');
                ylabel('intensity [A.U.]');
                text(0.7,0.9,['wx = ' num2str(gparamx(cnt,3)) ' nm' ], 'Units', 'normalized', 'FontSize', 8);
                text(0.7,0.7,['zmaxpos = ' num2str(zmaxpos) ' px' ], 'Units', 'normalized', 'FontSize', 6);

                subplot(3,3,8)
                cuty_fitZaxis=(0:0.2:length(cuty)).*deltaxy;
                cuty_fit=ffxy(gparamy(cnt, :), cuty_fitZaxis);
                plot(cutYaxis, cuty, 'r+', cuty_fitZaxis, cuty_fit, 'b-');
                xlabel('y [nm]');
                ylabel('intensity [A.U.]');
                text(0.7,0.9,['wy = ' num2str(gparamy(cnt,3)) ' nm' ], 'Units', 'normalized', 'FontSize', 8);
                text(0.7,0.7,['zmaxpos = ' num2str(zmaxpos) ' px' ], 'Units', 'normalized', 'FontSize', 6);

                
                orient(h, 'landscape');
                saveas(h, [output_prefix '.psf' num2str(cnt) '.pdf']);
                saveas(h, [output_prefix '.psf' num2str(cnt) '.fig']);
                save([output_prefix '.psf' num2str(cnt) '.mat'], 'beadout');
                
                if fit3Dgaussian                    
                    h=figure(3);
                    g=psf;
                    x0=fitresults_3D(1:3);
                    wxy=deltaxy;
                    wz=deltaz;
                    
                    [Ny Nx Nz]=size(psf);
                    
                    subplot(2,2,1);
                    cla
                    plot_gaussianprops3D_3D( [gparamx(cnt,2) gparamy(cnt,2) gparamz(cnt,2)], [gparamz(cnt,3) gparamx(cnt,3) gparamy(cnt,3)], 0, 0, 0,'xlim',[0 (Nx-1)*wxy],'ylim',[0 (Ny-1)*wxy],'zlim',[0 (Nz-1)*wz], 'faceColor',[0,0,1],'axisWidth',1,'faceAlpha', 0.3 );
                    hold on;
                    plot_gaussianprops3D_3D( fitresults_3D(1:3), fitresults_3D(4:6), fitresults_3D(7), fitresults_3D(8), fitresults_3D(9) , 'faceColor',[1,0,0],'axisWidth',2 )
                    hold off
                    grid on
                    set(gca,'YDir','normal');
                    set(gca,'ZDir','normal');
                    set(gca,'XDir','normal');
                    view(3);
                    xlabel('x-axis [nm]');
                    ylabel('y-axis [nm]');
                    zlabel('z-axis [nm]');
                    
                    title('blue: 1D fits,     red: 3D fit');
                    

                    subplot(2,2,2);
                    colormap('Gray')
                    %x0
                    %[round(x0(1)/wxy+1) round(x0(2)/wxy+1) round(x0(3)/wz+1)]
                    if (round(x0(2)/wxy+1)>0) && round(x0(2)/wxy+1)<=size(g,1)
                        dimgsc=squeeze(g(round(x0(2)/wxy+1),:,:))';
                        imagesc(cutXaxis, cutZaxis, dimgsc, [min(dimgsc(:)) max(dimgsc(:))]);
                        hold on
                        plot_gaussianprops3D_image( x0, 'xz', 1, 0, [gparamx(cnt,2) gparamy(cnt,2) gparamz(cnt,2)], [gparamz(cnt,3) gparamx(cnt,3) gparamy(cnt,3)], 0, 0, 0, 'color', 'b' )
                        plot_gaussianprops3D_image( x0, 'xz', fitresults_3D(10), fitresults_3D(11),fitresults_3D(1:3), fitresults_3D(4:6), fitresults_3D(7), fitresults_3D(8), fitresults_3D(9),'color', 'r' )
                        hold off
                        xlabel('x-axis [nm]');
                        ylabel('z-axis [nm]');
                        set(gca,'YDir','normal');
                        daspect([1 1 1]);
                    end
                    subplot(2,2,3);
                    if (round(x0(1)/wxy+1)>0) && round(x0(1)/wxy+1)<=size(g,2)
                        dimgsc=squeeze(g(:,round(x0(1)/wxy+1),:));
                        imagesc(cutZaxis, cutYaxis, dimgsc, [min(dimgsc(:)) max(dimgsc(:))]);
                        hold on
                        plot_gaussianprops3D_image( x0, 'zy', 1, 0, [gparamx(cnt,2) gparamy(cnt,2) gparamz(cnt,2)], [gparamz(cnt,3) gparamx(cnt,3) gparamy(cnt,3)], 0, 0, 0, 'color', 'b' )
                        plot_gaussianprops3D_image( x0, 'zy', fitresults_3D(10), fitresults_3D(11),fitresults_3D(1:3), fitresults_3D(4:6), fitresults_3D(7), fitresults_3D(8), fitresults_3D(9),'color', 'r' )
                        hold off
                        xlabel('z-axis [nm]');
                        ylabel('y-axis [nm]');
                        set(gca,'YDir','normal');
                        daspect([1 1 1]);
                    end
                    subplot(2,2,4);
                    if (round(x0(3)/wz+1)>0) && round(x0(3)/wz+1)<=size(g,3)
                        dimgsc=squeeze(g(:,:,round(x0(3)/wz+1)));
                        imagesc(cutXaxis, cutYaxis, dimgsc, [min(dimgsc(:)) max(dimgsc(:))]);
                        hold on
                        plot_gaussianprops3D_image( x0, 'xy', 1, 0, [gparamx(cnt,2) gparamy(cnt,2) gparamz(cnt,2)], [gparamz(cnt,3) gparamx(cnt,3) gparamy(cnt,3)], 0, 0, 0, 'color', 'b' )
                        plot_gaussianprops3D_image( x0, 'xy', fitresults_3D(10), fitresults_3D(11),fitresults_3D(1:3), fitresults_3D(4:6), fitresults_3D(7), fitresults_3D(8), fitresults_3D(9),'color', 'r' )
                        hold off
                        xlabel('x-axis [nm]');
                        ylabel('y-axis [nm]');
                        set(gca,'YDir','normal');
                        daspect([1 1 1]);
                    end
                    orient(h, 'landscape');
                    saveas(h, [output_prefix '.psf' num2str(cnt) '_3D.pdf']);
                    saveas(h, [output_prefix '.psf' num2str(cnt) '_3D.fig']);
                end
                pause(0.1)
            end
            
            disp([num2str(cnt) '/' num2str(cnt1) ':   initial bead position: ' num2str(psf_positions(cnt,:))]);
            disp(['    initial abs position: ' num2str((psf_positions(cnt,:)-[1 1 1]).*[deltaxy deltaxy deltaz])]);
            if fit3Dgaussian
                disp(['    output fit position:  [' num2str(gabspos(cnt,:)) ']       3D-fit:  [' num2str(beadout.beadpos_fit3D') ']']);
                disp(['    output fit widths:  [' num2str(sort([gparamx(cnt,3), gparamy(cnt,3), gparamz(cnt,3)])) ']       3D-fit:  [' num2str(sort(beadout.beadwidths_fit3D')) ']']);
            else
                disp(['    output fit position:  [' num2str(gabspos(cnt,:)) ']']);
                disp(['    output fit widths:  [' num2str(sort([gparamx(cnt,3), gparamy(cnt,3), gparamz(cnt,3)])) ']']);
            end
            %cnt
            
            cnt=cnt+1;
        end
    end

    dataout.gparamx=gparamx;
    dataout.gparamy=gparamy;
    dataout.gparamz=gparamz;
    dataout.gabspos=gabspos;
    
    if fit3Dgaussian
        dataout.gparam3D=gparam3D;
        dataout.gabspos3D=gabspos3D;
    end
    
    
    wz=abs(gparamz(:,3));
    wx=abs(gparamx(:,3));
    wy=abs(gparamy(:,3));
    
    idx=find(wx>=histwidthxyX(1) & wx<histwidthxyX(end) & wy>=histwidthxyX(1) & wy<=histwidthxyX(end) & wz>=histwidthzX(1) & wz<=histwidthzX(end));
    wx=wx(idx);
    wy=wy(idx);
    wz=wz(idx);
    dataout.inrange_idx=idx;

    disp(['avg(wx) = ' num2str(mean(wx)) '    median(wx) = ' num2str(median(wx)) '    SD(wx) = ' num2str(std(wx))  ]);
    disp(['avg(wy) = ' num2str(mean(wy)) '    median(wy) = ' num2str(median(wy)) '    SD(wy) = ' num2str(std(wy))  ]);
    disp(['avg(wz) = ' num2str(mean(wz)) '    median(wz) = ' num2str(median(wz)) '    SD(wz) = ' num2str(std(wz))  ]);

    disp(['avg(gamma_zx) = ' num2str(mean(wz./wx)) '    median(gamma_zx) = ' num2str(median(wz./wx)) '    SD(gamma_zx) = ' num2str(std(wz./wx))  ]);
    disp(['avg(gamma_zy) = ' num2str(mean(wz./wy)) '    median(gamma_zy) = ' num2str(median(wz./wy)) '    SD(gamma_zy) = ' num2str(std(wz./wy))  ]);

    h=figure(14);
    subplot(3,3,1)
    hist(wx, histwidthxyX);
    xlabel('x width [nm]');
    ylabel('frequency');
    title([ 'N_PSF = ' num2str(length(wx)) ' ( out of ' num2str(length(psf_positions)) ')'], 'Interpreter', 'none')
    text(0.5,0.94,['avg(wx) = ' num2str(mean(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wx) = ' num2str(median(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wx) = ' num2str(std(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,2)
    hist(wy, histwidthxyX);
    xlabel('y width [nm]');
    ylabel('frequency');
    text(0.5,0.94,['avg(wy) = ' num2str(mean(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wy) = ' num2str(median(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wY) = ' num2str(std(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,3)
    hist(wz, histwidthzX);
    xlabel('z width [nm]');
    ylabel('frequency');
    text(0.5,0.94,['avg(wz) = ' num2str(mean(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wz) = ' num2str(median(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wz) = ' num2str(std(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,6)
    plot(psf_positions(idx,3), wx, 'r+');
    hold on
    plot(psf_positions(idx,3), wy, 'g+');
    plot(psf_positions(idx,3), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('z-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');
    subplot(3,3,4)
    plot(psf_positions(idx,1), wx, 'r+');
    hold on
    plot(psf_positions(idx,1), wy, 'g+');
    plot(psf_positions(idx,1), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('x-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');
    subplot(3,3,5)
    plot(psf_positions(idx,2), wx, 'r+');
    hold on
    plot(psf_positions(idx,2), wy, 'g+');
    plot(psf_positions(idx,2), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('y-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');

    subplot(3,3,7)
    hist(wz./wx, histgammaX);
    xlabel('axial ratio w_z/w_x');
    ylabel('frequency');
    text(0.5,0.94,['avg(gamma_zx) = ' num2str(mean(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(gamma_zx) = ' num2str(median(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(gamma_zx) = ' num2str(std(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,8)
    hist(wz./wy, histgammaX);
    xlabel('axial ratio w_z/w_y');
    ylabel('frequency');
    text(0.5,0.94,['avg(gamma_zy) = ' num2str(mean(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(gamma_zy) = ' num2str(median(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(gamma_zy) = ' num2str(std(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);

    
    dataout.wx=wx;
    dataout.wy=wy;
    dataout.wz=wz;
    dataout.used_psf_positions=psf_positions(idx,:);



    orient(h, 'landscape');
    saveas(h, [output_prefix '.psfhistogram.pdf']);
    saveas(h, [output_prefix '.psfhistogram.fig']);
    M=[psf_positions(idx,1)';psf_positions(idx,2)';psf_positions(idx,3)';wx';wy';wz']';
    dlmwrite([output_prefix '.psfwidths.dat'], M);
    
    if fit3Dgaussian
        dataout.gparam3D=gparam3D;
        dataout.gabspos3D=gabspos3D;
        h3=figure(21);


        w=abs(gparam3D(:,4:6));
        sort(w,2);
        wx=w(:,3);
        wy=w(:,2);
        wz=w(:,1);

        idx=find(wx>=histwidthxyX(1) & wx<histwidthxyX(end) & wy>=histwidthxyX(1) & wy<=histwidthxyX(end) & wz>=histwidthzX(1) & wz<=histwidthzX(end));
        wx=wx(idx);
        wy=wy(idx);
        wz=wz(idx);
        dataout.inrange_idx=idx;

        disp(['avg(wsmall) = ' num2str(mean(wx)) '    median(wsmall) = ' num2str(median(wx)) '    SD(wsmall) = ' num2str(std(wx))  ]);
        disp(['avg(wmid) = ' num2str(mean(wy)) '    median(wmid) = ' num2str(median(wy)) '    SD(wmid) = ' num2str(std(wy))  ]);
        disp(['avg(wlarge) = ' num2str(mean(wz)) '    median(wlarge) = ' num2str(median(wz)) '    SD(wlarge) = ' num2str(std(wz))  ]);

        disp(['avg(gamma_zsmall) = ' num2str(mean(wz./wx)) '    median(gamma_zsmall) = ' num2str(median(wz./wx)) '    SD(gamma_zsmall) = ' num2str(std(wz./wx))  ]);
        disp(['avg(gamma_zmid) = ' num2str(mean(wz./wy)) '    median(gamma_zmid) = ' num2str(median(wz./wy)) '    SD(gamma_zmid) = ' num2str(std(wz./wy))  ]);

        subplot(3,3,1)
        hist(wx, histwidthxyX);
        xlabel('smallest width [nm]');
        ylabel('frequency');
        title([ 'N_PSF = ' num2str(length(wx)) ' ( out of ' num2str(length(psf_positions)) ')'], 'Interpreter', 'none')
        text(0.5,0.94,['avg(wsmall) = ' num2str(mean(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wsmall) = ' num2str(median(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wsmall) = ' num2str(std(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(3,3,2)
        hist(wy, histwidthxyX);
        xlabel('mid width [nm]');
        ylabel('frequency');
        text(0.5,0.94,['avg(wmid) = ' num2str(mean(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wmid) = ' num2str(median(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wmid) = ' num2str(std(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(3,3,3)
        hist(wz, histwidthzX);
        xlabel('largest width [nm]');
        ylabel('frequency');
        text(0.5,0.94,['avg(wlarge) = ' num2str(mean(wz))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wlarge) = ' num2str(median(wz))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wlarge) = ' num2str(std(wz))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(3,3,6)
        plot(psf_positions(idx,3), wx, 'r+');
        hold on
        plot(psf_positions(idx,3), wy, 'g+');
        plot(psf_positions(idx,3), wz, 'b+');
        hold off
        legend('smallest width', 'mid. width', 'largest width');
        xlabel('z-position of psf measurement [pixel]');
        ylabel('width of psf [µm]');
        subplot(3,3,4)
        plot(psf_positions(idx,1), wx, 'r+');
        hold on
        plot(psf_positions(idx,1), wy, 'g+');
        plot(psf_positions(idx,1), wz, 'b+');
        hold off
        legend('smallest width', 'mid. width', 'largest width');
        xlabel('x-position of psf measurement [pixel]');
        ylabel('width of psf [µm]');
        subplot(3,3,5)
        plot(psf_positions(idx,2), wx, 'r+');
        hold on
        plot(psf_positions(idx,2), wy, 'g+');
        plot(psf_positions(idx,2), wz, 'b+');
        hold off
        legend('smallest width', 'mid. width', 'largest width');
        xlabel('y-position of psf measurement [pixel]');
        ylabel('width of psf [µm]');

        subplot(3,3,7)
        hist(wz./wx, histgammaX);
        xlabel('axial ratio w_{large}/w_{small}');
        ylabel('frequency');
        text(0.5,0.94,['avg(gamma_{largesmall}) = ' num2str(mean(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(gamma_{largesmall}) = ' num2str(median(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(gamma_{largesmall}) = ' num2str(std(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(3,3,8)
        hist(wz./wy, histgammaX);
        xlabel('axial ratio w_{large}/w_{mid}');
        ylabel('frequency');
        text(0.5,0.94,['avg(gamma_{largemid}) = ' num2str(mean(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(gamma_{largemid}) = ' num2str(median(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(gamma_{largemid}) = ' num2str(std(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(3,3,9)
        hist(wx./wy, histgammaX);
        xlabel('axial ratio w_{small}/w_{mid}');
        ylabel('frequency');
        text(0.5,0.94,['avg(gamma_{smallmid}) = ' num2str(mean(wx./wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(gamma_{smallmid}) = ' num2str(median(wx./wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(gamma_{smallmid}) = ' num2str(std(wx./wy))  ], 'Units', 'normalized', 'FontSize', 6);
    
    
        orient(h, 'landscape');
        saveas(h3, [output_prefix '.psfhistogram3D.pdf']);
        saveas(h3, [output_prefix '.psfhistogram3D.fig']);
        
        M=[psf_positions(idx,1)';psf_positions(idx,2)';psf_positions(idx,3)';wx';wy';wz']';
        dlmwrite([output_prefix '.psfwidths3D.dat'], M);

    end
    
     if fit3Dgaussian
         
        param3D=gparam3D;
        pos3D=gabspos3D;
        dataout.gparam3D=gparam3D;
        dataout.gabspos3D=gabspos3D;
        
        h4=figure(22);
        
        
        subplot(3,3,1)
        plot_points_binlinfit(pos3D(:,1)/1000, param3D(:, 4), 20, 0.05,  'x-position [µm]', 'largest width [nm]', 'FontSize', 6);
        subplot(3,3,2)
        plot_points_binlinfit(pos3D(:,1)/1000, param3D(:, 5), 20, 0.05,  'x-position [µm]', 'mid width [nm]', 'FontSize', 6);
        subplot(3,3,3)
        plot_points_binlinfit(pos3D(:,1)/1000, param3D(:, 6), 20, 0.05,  'x-position [µm]', 'smallest width [nm]', 'FontSize', 6);
        
        subplot(3,3,4)
        plot_points_binlinfit(pos3D(:,2)/1000, param3D(:, 4), 20, 0.05,  'y-position [µm]', 'largest width [nm]', 'FontSize', 6);
        subplot(3,3,5)
        plot_points_binlinfit(pos3D(:,2)/1000, param3D(:, 5), 20, 0.05,  'y-position [µm]', 'mid width [nm]', 'FontSize', 6);
        subplot(3,3,6)
        plot_points_binlinfit(pos3D(:,2)/1000, param3D(:, 6), 20, 0.05,  'y-position [µm]', 'smallest width [nm]', 'FontSize', 6);
        
        subplot(3,3,7)
        plot_points_binlinfit(pos3D(:,3)/1000, param3D(:, 4), 20, 0.05,  'z-position [µm]', 'largest width [nm]', 'FontSize', 6);
        subplot(3,3,8)
        plot_points_binlinfit(pos3D(:,3)/1000, param3D(:, 5), 20, 0.05,  'z-position [µm]', 'mid width [nm]', 'FontSize', 6);
        subplot(3,3,9)
        plot_points_binlinfit(pos3D(:,3)/1000, param3D(:, 6), 20, 0.05,  'z-position [µm]', 'smallest width [nm]', 'FontSize', 6);
        
    
        orient(h, 'tall');
        saveas(h4, [output_prefix '.psf_posdependency3D.pdf']);
        saveas(h4, [output_prefix '.psf_posdependency3D.fig']);
        
    end

    if fitxzcolumns
        
        xzcolumns_gaussianwidthfit_x(cnt,:)=xp;
        xzcolumns_gaussianwidthfit_y(cnt,:)=yp;

        
        dataout.gparam3D=gparam3D;
        dataout.gabspos3D=gabspos3D;
        hx=figure(31);


        w=abs(gparam3D(:,4:6));
        sort(w,2);
        wx=xzcolumns_gaussianwidthfit_x(:,1);
        wy=xzcolumns_gaussianwidthfit_y(:,1);
        wzx=xzcolumns_gaussianwidthfit_x(:,2);
        wzy=xzcolumns_gaussianwidthfit_y(:,2);

        idx=find(wx>=histwidthxyX(1) & wx<histwidthxyX(end) & wy>=histwidthxyX(1) & wy<=histwidthxyX(end) & wzx>=histwidthzX(1) & wzx<=histwidthzX(end) & wzy>=histwidthzX(1) & wzy<=histwidthzX(end));
        wx=wx(idx);
        wy=wy(idx);
        wzx=wzx(idx);
        wzy=wzy(idx);
        dataout.xzcolumns.inrange_idx=idx;
        dataout.xzcolumns.wx=wx;
        dataout.xzcolumns.wy=wy;
        dataout.xzcolumns.wzx=wzx;
        dataout.xzcolumns.wzy=wzy;

        disp(['avg(wx) = ' num2str(mean(wx)) '    median(wx) = ' num2str(median(wx)) '    SD(wx) = ' num2str(std(wx))  ]);
        disp(['avg(wy) = ' num2str(mean(wy)) '    median(wy) = ' num2str(median(wy)) '    SD(wy) = ' num2str(std(wy))  ]);
        disp(['avg(wz_x) = ' num2str(mean(wzx)) '    median(wz_x) = ' num2str(median(wzx)) '    SD(wz_x) = ' num2str(std(wzx))  ]);
        disp(['avg(wz_y) = ' num2str(mean(wzy)) '    median(wz_y) = ' num2str(median(wzy)) '    SD(wz_y) = ' num2str(std(wzy))  ]);

        subplot(2,2,1)
        hist(wx, histwidthxyX);
        xlabel('wx [nm]');
        ylabel('frequency');
        title([ 'N_PSF = ' num2str(length(wx)) ' ( out of ' num2str(length(psf_positions)) ')'], 'Interpreter', 'none')
        text(0.5,0.94,['avg(wx) = ' num2str(mean(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wx) = ' num2str(median(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wx) = ' num2str(std(wx))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(2,2,2)
        hist(wy, histwidthxyX);
        xlabel('wy [nm]');
        ylabel('frequency');
        text(0.5,0.94,['avg(wy) = ' num2str(mean(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wy) = ' num2str(median(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wy) = ' num2str(std(wy))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(2,2,3)
        hist(wzx, histwidthzX);
        xlabel('wz_x [nm]');
        ylabel('frequency');
        text(0.5,0.94,['avg(wz_x) = ' num2str(mean(wzx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wz_x) = ' num2str(median(wzx))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wz_x) = ' num2str(std(wzx))  ], 'Units', 'normalized', 'FontSize', 6);
        subplot(2,2,4)
        hist(wzy, histwidthzX);
        xlabel('wz_y [nm]');
        ylabel('frequency');
        text(0.5,0.94,['avg(wz_y) = ' num2str(mean(wzy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.87,['median(wz_y) = ' num2str(median(wzy))  ], 'Units', 'normalized', 'FontSize', 6);
        text(0.5,0.8,['SD(wz_y) = ' num2str(std(wzy))  ], 'Units', 'normalized', 'FontSize', 6);

        orient(h, 'tall');
        saveas(hx, [output_prefix '.psfhistogram_xzcol.pdf']);
        saveas(hx, [output_prefix '.psfhistogram_xzcol.fig']);
        
        M=[psf_positions(idx,1)';psf_positions(idx,2)';psf_positions(idx,3)';wx';wy';wzx';wzy']';
        dlmwrite([output_prefix '.psfwidths_xzcol.dat'], M);

    end
    
    save([output_prefix '.psfhistogram.mat'], 'dataout');

    output_args=dataout;

end


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

function evaluate_dualcolorzstackresults( dualcolor_stack, output_prefix, varargin )

    if (length(dualcolor_stack)>1)



        histwidthxyrange=[400 1500];
        histwidthzrange=[400 1500];
        histwidthxybinwidth=10;
        histwidthzbinwidth=10;
        posHistBins=50;
        maxDualcolorBeadDistanceXY=200;
        maxDualcolorBeadDistanceZ=500;
        has3Dfit=false;


        optargin = size(varargin,2);
        for a=1:2:optargin
            name= varargin{a};
            val= varargin{a+1};

            if (strcmp(name, 'histwidthxyrange') )
                histwidthxyrange=val;
            elseif (strcmp(name, 'histwidthzrange') )
                histwidthzrange=val;
            elseif (strcmp(name, 'histwidthxybinwidth') )
                histwidthxybinwidth=val;
            elseif (strcmp(name, 'histwidthzbinwidth') )
                histwidthzbinwidth=val;
            elseif (strcmp(name, 'poshistbins') )
                posHistBins=val;
            elseif (strcmp(name, 'maxdualcolorbeaddistancexy') )
                maxDualcolorBeadDistanceXY=val;
            elseif (strcmp(name, 'maxdualcolorbeaddistancez') )
                maxDualcolorBeadDistanceZ=val;
            elseif (strcmp(name, 'has3Dfit') )
                has3Dfit=val;
            else
                error(['unknown parameter "' name '" to evaluate_dualcolorzstackresults()']);
            end
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


        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % extract the bead positions and widths from the input data for both
        % color channels and select from these ony those beads that returned a
        % proper fit in both color channels!
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        [N Nparam]=size(dualcolor_stack{1}.gparamx);
        dualcolor_stack{1}.inrange_idx
        dualcolor_stack{2}.inrange_idx
        idx1=false(N,1);
        idx1(dualcolor_stack{1}.inrange_idx)=true;
        idx2=false(N,1);
        idx2(dualcolor_stack{2}.inrange_idx)=true;
        idx= idx1 & idx2;
        Nidx=sum(idx);
        dataout.Nidx=Nidx;
        dataout.idx=idx;

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % calculate the diference in bead position in the two color channels
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        %xposdist=dualcolor_stack{1}.gparamx(idx,2)-dualcolor_stack{2}.gparamx(idx,2);
        %yposdist=dualcolor_stack{1}.gparamy(idx,2)-dualcolor_stack{2}.gparamy(idx,2);
        %zposdist=dualcolor_stack{1}.gparamz(idx,2)-dualcolor_stack{2}.gparamz(idx,2);
        xposdist=dualcolor_stack{1}.gabspos(idx,1)-dualcolor_stack{2}.gabspos(idx,1);
        yposdist=dualcolor_stack{1}.gabspos(idx,2)-dualcolor_stack{2}.gabspos(idx,2);
        zposdist=dualcolor_stack{1}.gabspos(idx,3)-dualcolor_stack{2}.gabspos(idx,3);

        posgoodidx = (abs(xposdist)<maxDualcolorBeadDistanceXY) ...
                     & (abs(yposdist)<maxDualcolorBeadDistanceXY) ...
                     & (abs(zposdist)<maxDualcolorBeadDistanceZ);
        Nposgoodidx=sum(posgoodidx);
        dataout.Nposgoodidx=Nposgoodidx;
        dataout.posgoodidx=posgoodidx;
        dataout.maxDualcolorBeadDistanceXY=maxDualcolorBeadDistanceXY;
        dataout.maxDualcolorBeadDistanceZ=maxDualcolorBeadDistanceZ;
        dataout.xposdist=xposdist; dataout.yposdist=yposdist; dataout.zposdist=zposdist; 

        [xposhistData xposhistX]=hist(xposdist(posgoodidx), posHistBins);
        dataout.xposhistData=xposhistData; dataout.xposhistX=xposhistX; dataout.posHistBins=posHistBins;
        [yposhistData yposhistX]=hist(yposdist(posgoodidx), posHistBins);
        dataout.yposhistData=yposhistData; dataout.yposhistX=yposhistX; dataout.posHistBins=posHistBins;
        [zposhistData zposhistX]=hist(zposdist(posgoodidx), posHistBins);
        dataout.zposhistData=zposhistData; dataout.zposhistX=zposhistX; dataout.posHistBins=posHistBins;

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % calculate histograms of the bead widths in both color channels
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
        wx1=dualcolor_stack{1}.gparamx(idx,3);
        wx2=dualcolor_stack{2}.gparamx(idx,3);
        [wx1hist_data]=hist(wx1,histwidthxyX);
        [wx2hist_data]=hist(wx2,histwidthxyX);
        dataout.wx1=wx1; dataout.wx2=wx2;
        dataout.wx1hist_data=wx1hist_data; dataout.wx2hist_data=wx2hist_data;

        wy1=dualcolor_stack{1}.gparamy(idx,3);
        wy2=dualcolor_stack{2}.gparamy(idx,3);
        [wy1hist_data]=hist(wy1,histwidthxyX);
        [wy2hist_data]=hist(wy2,histwidthxyX);
        dataout.wy1=wy1; dataout.wy2=wy2;
        dataout.wy1hist_data=wy1hist_data; dataout.wy2hist_data=wy2hist_data;

        wz1=dualcolor_stack{1}.gparamz(idx,3);
        wz2=dualcolor_stack{2}.gparamz(idx,3);
        [wz1hist_data]=hist(wz1,histwidthzX);
        [wz2hist_data]=hist(wz2,histwidthzX);
        dataout.wz1=wz1; dataout.wz2=wz2;
        dataout.wz1hist_data=wz1hist_data; dataout.wz2hist_data=wz2hist_data;



        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot the distributions of widthes and the distance histograms
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        h=figure(10);
        subplot(3,2,1);
        stairs(histwidthxyX, wx1hist_data, 'b-');
        hold on
        stairs(histwidthxyX, wx2hist_data, 'r-');
        xlabel('wx [nm]');
        ylabel('frequency');
        title(['distribution of PSF x-width [N = ' num2str(Nidx) ']']);
        text(0.5,0.95,['avg(wx_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(mean(wx1)) '\color{red}    ' num2str(mean(wx2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.8,['median(wx_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(median(wx1)) '\color{red}    ' num2str(median(wx2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.65,['SD(wx_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(std(wx1)) '\color{red}    ' num2str(std(wx2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');

        hold off


        subplot(3,2,2);
        stairs(histwidthxyX, wy1hist_data, 'b-');
        hold on
        stairs(histwidthxyX, wy2hist_data, 'r-');
        xlabel('wy [nm]');
        ylabel('frequency');
        title(['distribution of PSF y-width [N = ' num2str(Nidx) ']']);
        text(0.5,0.95,['avg(wy_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(mean(wy1)) '\color{red}    ' num2str(mean(wy2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.8,['median(wy_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(median(wy1)) '\color{red}    ' num2str(median(wy2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.65,['SD(wy_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(std(wy1)) '\color{red}    ' num2str(std(wy2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        hold off

        subplot(3,2,3);
        stairs(histwidthzX, wz1hist_data, 'b-');
        hold on
        stairs(histwidthzX, wz2hist_data, 'r-');
        xlabel('wz [nm]');
        ylabel('frequency');
        title(['distribution of PSF z-width [N = ' num2str(Nidx) ']']);
        text(0.5,0.95,['avg(wz_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(mean(wz1)) '\color{red}    ' num2str(mean(wz2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.8,['median(wz_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(median(wz1)) '\color{red}    ' num2str(median(wz2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.5,0.65,['SD(wz_{\color{blue}1,\color{red}2}) =\color{blue} ' num2str(std(wz1)) '\color{red}    ' num2str(std(wz2))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        hold off

        subplot(3,2,4);
        bar(xposhistX, xposhistData, 'r');
        xlabel('\delta x=x_1-x_2 [nm]');
        ylabel('frequency');
        title(['distribution of PSF distances [N = ' num2str(Nposgoodidx) ']']);
        text(0.1,0.95,['avg(\delta x) = ' num2str(mean(xposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.8,['median(\delta x) = ' num2str(median(xposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.65,['SD(\delta x) = ' num2str(std(xposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');


        subplot(3,2,5);
        bar(yposhistX, yposhistData, 'r');
        xlabel('\delta y=y_1-y_2 [nm]');
        ylabel('frequency');
        title(['distribution of PSF distances [N = ' num2str(Nposgoodidx) ']']);
        text(0.1,0.95,['avg(\delta y) = ' num2str(mean(yposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.8,['median(\delta y) = ' num2str(median(yposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.65,['SD(\delta y) = ' num2str(std(yposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');


        subplot(3,2,6);
        bar(zposhistX, zposhistData, 'r');
        xlabel('\delta z=z_1-z_2 [nm]');
        ylabel('frequency');
        title(['distribution of PSF distances [N = ' num2str(Nposgoodidx) ']']);
        text(0.1,0.95,['avg(\delta z) = ' num2str(mean(zposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.8,['median(\delta z) = ' num2str(median(zposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');
        text(0.1,0.65,['SD(\delta z) = ' num2str(std(zposdist(posgoodidx)))  ], 'Units', 'normalized', 'FontSize', 7, 'BackgroundColor', 'w');

        orient(h, 'tall');
        saveas(h, [output_prefix 'dualcolor_comparewidpos.pdf']);
        saveas(h, [output_prefix 'dualcolor_comparewidpos.fig']);




        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot the position dependence of the distances
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        h=figure(11);
        subplot(3,3,1);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,1)./1000, xposdist(posgoodidx), 'b+');
        xlabel('x position µmnm]');
        ylabel('\delta x [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,1)'./1000, xposdist(posgoodidx)');
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,2);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, xposdist(posgoodidx), 'b+');
        xlabel('y position [µm]');
        ylabel('\delta x [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, xposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,3);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, xposdist(posgoodidx), 'b+');
        xlabel('z position [µm]');
        ylabel('\delta x [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, xposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');

        subplot(3,3,4);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,1)./1000, yposdist(posgoodidx), 'b+');
        xlabel('x position [µm]');
        ylabel('\delta y [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,1)./1000, yposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,5);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, yposdist(posgoodidx), 'b+');
        xlabel('y position [µm]');
        ylabel('\delta y [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, yposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,6);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, yposdist(posgoodidx), 'b+');
        xlabel('z position [µm]');
        ylabel('\delta y [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, yposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');

        subplot(3,3,7);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,1)./1000, zposdist(posgoodidx), 'b+');
        xlabel('x position [µm]');
        ylabel('\delta z [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,1)./1000, zposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,8);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, zposdist(posgoodidx), 'b+');
        xlabel('y position [µm]');
        ylabel('\delta z [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,2)./1000, zposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');
        subplot(3,3,9);
        plot(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, zposdist(posgoodidx), 'b+');
        xlabel('z position [µm]');
        ylabel('\delta z [nm]');
        cc=corrcoef(dualcolor_stack{1}.gabspos(posgoodidx,3)./1000, zposdist(posgoodidx));
        text(0.1,0.94,['corrcoef = ' num2str(cc(1,2)*100) '%'  ], 'Units', 'normalized', 'FontSize', 8, 'BackgroundColor', 'w');

        orient(h, 'tall');
        saveas(h, [output_prefix 'dualcolor_pos.pdf']);
        saveas(h, [output_prefix 'dualcolor_pos.png']);
        saveas(h, [output_prefix 'dualcolor_pos.fig']);



        save([output_prefix 'dualcolor_compare.mat'], 'dataout');
    else
        disp('bead data contains only one color channel ... SKIPPING DUAL COLOR EVALUATION!');
    end

end


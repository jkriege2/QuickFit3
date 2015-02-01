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

function [ output_args ] = perform_lightsheetanalysis(zpositions, frames, result_basename, varargin )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    f=frames;
    optargin = size(varargin,2);

    mode='fitavg';%'fitall';
    initparammode='takedefault'; %'takelast'
    imagerange=[];
    expectedWidthRange=[1 3];
    pixsize=[1 1 1];
    fitallsteps=1;
    maxfunevals=5000;
    maxiter=1000;
    fitallmode='single';
    widthrange=[];
    saveintermediates=true;

    outputdirectory='';

    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'initparammode') )
            initparammode=val;
        elseif (strcmp(name, 'mode') )
            mode=val;
        elseif (strcmp(name, 'imagerange') )
            if (~isempty(val)) 
                imagerange=val;
            end
        elseif (strcmp(name, 'expectedWidthRange') )
            if (~isempty(val)) 
                expectedWidthRange=val;
            end
        elseif (strcmp(name, 'pixsize') )
            pixsize=val;
        elseif (strcmp(name, 'fitallsteps') )
            fitallsteps=val;
        elseif (strcmp(name, 'maxfunevals') )
            maxfunevals=val;
        elseif (strcmp(name, 'maxiter') )
            maxiter=val;
        elseif (strcmp(name, 'widthrange') )
            widthrange=val;
        elseif (strcmp(name, 'outputdirectory') )
            outputdirectory=val;
        elseif (strcmp(name, 'saveintermediates') )
            saveintermediates=val;
        end
    end

    [w h N]=size(f);

    if (isempty(imagerange))
        imagerange=1:N;
    end
    if (isempty(widthrange))
        widthrange=[1 w];
    end    %imagerange
    nparams=4;
    t=zpositions.*pixsize(3);

    pmean(1:N,1:nparams)=0;
    pstd(1:N,1:nparams)=0;
    pp(1:N,1:h,1:nparams)=0;
    for idx=imagerange
        if (strcmp(mode, 'fitall')) 
            for hh=1:fitallsteps:h
                wr=widthrange(1):widthrange(2);
                if (strcmp(fitallmode, 'avg'))
                    datay=mean(f(wr,hh:(min(ff+fitallsteps-1, h)),idx), 2);
                else
                    datay=f(wr,hh,idx);
                end
                datax=pixsize(1).*(wr)';
                [m mi]=max(datay);
                p0=[m-min(datay), datax(mi(1)), 2, min(datay)];
                if idx>imagerange(1) && strcmp(initparammode, 'takelast')
                    p0= pp(idx-1,1,:);
                end
                
                options = optimset('lsqcurvefit');
                options = optimset(options, 'MaxFunEvals',maxfunevals);
                options = optimset(options, 'MaxIter',maxiter);

                pp(idx,hh,:) = lsqcurvefit(@fit_gauss1Dvar,p0,datax,datay, [], [], options);

                pp(idx,hh,3)=sqrt(abs(pp(idx,hh,3)));
                

                if mod(hh,int16(h/fitallsteps/3))==0
                    fh=figure(2);
                    fitx=pixsize(1).*(widthrange(1):((widthrange(2)-widthrange(1))/length(wr)/4):widthrange(2))';
                    plot(datax, datay);
                    xlabel('position [µm]');
                    ylabel('intensity [A.U.]');
                    hold on;
                    fity=fit_gauss1D(pp(idx,hh,:), fitx);
                    plot(fitx, fity, 'r-');
                    hold off;
                    if (saveintermediates)
                        drawnow
                        saveas(2, [ outputdirectory result_basename '_n' num2str(idx) '_fitr' num2str(hh) '.pdf']);
                        saveas(2, [ outputdirectory result_basename '_n' num2str(idx) '_fitr' num2str(hh) '.fig']);
                    else
                        drawnow
                    end
                    %close(fh);
                end
            end
            pmean(idx,:)=mean(pp(idx,1:fitallsteps:h,:));
            pstd(idx,:)=std(pp(idx,1:fitallsteps:h,:));
        elseif (strcmp(mode, 'fitavg')) 
            wr=widthrange(1):widthrange(2);
            datay=mean(f(wr,1:h,idx),2);
            datax=pixsize(1).*(wr)';
            [m mi]=max(datay);
            p0=[m-min(datay), datax(mi(1)), 2, min(datay)];
            if idx>imagerange(1) && strcmp(initparammode, 'takelast')
                p0= pp(idx-1,1,:);
            end

            options = optimset('lsqcurvefit');
            options = optimset(options, 'MaxFunEvals',maxfunevals);
            options = optimset(options, 'MaxIter',maxiter);
            pp(idx,1,:) = lsqcurvefit(@fit_gauss1Dvar,p0,datax,datay,[],[],options);
            pp(idx,1,3)=sqrt(abs(pp(idx,1,3)));

            fh=figure(2);
            fitx=pixsize(1).*(widthrange(1):((widthrange(2)-widthrange(1))/length(wr)/4):widthrange(2))';
            plot(datax, datay);
            xlabel('position [µm]');
            ylabel('intensity [A.U.]');
            hold on;
            fity=fit_gauss1D(pp(idx,1,:), fitx);
            plot(fitx, fity, 'r-');
            hold off;
            drawnow;
            if (saveintermediates)
                saveas(2, [outputdirectory result_basename '_n' num2str(idx) '_fitavg.pdf']);
                saveas(2, [outputdirectory  result_basename '_n' num2str(idx) '_fitavg.fig']);
            end
            %close(fh);
            pmean(idx,:)=pp(idx,1,:);
            pstd(idx,:)=0;%std(pp(idx,1,:));
        end

        figure(1)
        subplot(2,1,1);
        errorbar(t, pmean(:,2), pstd(:,2), '+');
        xlabel('beam position [µm]');
        ylabel('peak position [µm]');
        title('beam position');
        ylim([-pixsize(1)*w*0.05 pixsize(1)*w*1.05] )

        subplot(2,1,2);
        errorbar(t, pmean(:,3), pstd(:,3), '+');
        xlabel('beam position [µm]');
        ylabel('1/e² peak width [µm]');
        title('1/e² beam width');
        ylim(expectedWidthRange);
        saveas(1, [ outputdirectory  result_basename '.pdf']);
        saveas(1, [ outputdirectory  result_basename '.fig']);
    end

    save([ outputdirectory  result_basename '.mat']);

    idx=imagerange;
    figure(1)
    subplot(2,1,1);
    errorbar(t(idx), pmean(idx,2), pstd(idx,2), '+');
    xlabel('beam position [µm]');
    ylabel('peak position [µm]');
    title('beam position');

    subplot(2,1,2);
    errorbar(t(idx), pmean(idx,3), pstd(idx,3), '+');
    xlabel('beam position [µm]');
    ylabel('1/e² peak width [µm]');
    title('1/e² beam width');
    saveas(1, [outputdirectory  result_basename '.pdf']);
    saveas(1, [outputdirectory  result_basename '.fig']);

    idxs=imagerange;
    figure(3)
    subplot(2,1,1);
    errorbar(t(idxs), pmean(idxs,2), pstd(idxs,2), '+');
    xlabel('beam position [µm]');
    ylabel('peak position [µm]');
    title('beam position');

    subplot(2,1,2);
    errorbar(t(idxs), pmean(idxs,3), pstd(idxs,3), '+');
    xlabel('beam position [µm]');
    ylabel('1/e² peak width [µm]');
    title('1/e² beam width');
    saveas(3, [outputdirectory  result_basename '_detail.pdf']);
    saveas(3, [outputdirectory  result_basename '_detail.fig']);

    
    firstidx=idxs(1);
    mpos=find((pmean(idxs,3)==min(pmean(idxs,3))));
    mwidth=floor(max(imagerange)/2);
    idxs=(firstidx+mpos(1)-mwidth):(firstidx+mpos(1)+mwidth);
    idxs_notoutofrange=find( (idxs>=min(imagerange)) & (idxs<=max(imagerange)) );
    idxs=idxs(idxs_notoutofrange);
    figure(4)
    subplot(2,1,1);
    errorbar(t(idxs), pmean(idxs,2), pstd(idxs,2), '+');
    xlabel('beam position [µm]');
    ylabel('peak position [µm]');
    title('beam position');

    subplot(2,1,2);
    errorbar(t(idxs), pmean(idxs,3), pstd(idxs,3), '+');
    xlabel('beam position [µm]');
    ylabel('1/e² peak width [µm]');
    title('1/e² beam width');
    ylim(expectedWidthRange);
    saveas(4, [outputdirectory  result_basename '_detaily.pdf']);
    saveas(4, [outputdirectory  result_basename '_detaily.fig']);



end


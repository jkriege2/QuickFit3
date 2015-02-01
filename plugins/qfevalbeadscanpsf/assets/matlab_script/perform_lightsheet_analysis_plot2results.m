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

function perform_lightsheet_analysis_plot2results(directory, output_basename, result1_basename, result1_name, result2_basename, result2_name, varargin)


    optargin = size(varargin,2);

    detail_range1=[];
    detail_range2=[];
    expectedWidthRange=[1 2];
    outputdirectory='';

    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'detail_range1') )
            if (~isempty(val)) 
                detail_range1=val;
            end
        elseif (strcmp(name, 'detail_range2') )
            if (~isempty(val)) 
                detail_range2=val;
            end
        elseif (strcmp(name, 'expectedWidthRange') )
            if (~isempty(val)) 
                expectedWidthRange=val;
            end
        elseif (strcmp(name, 'outputdirectory') )
            outputdirectory=val;
        end
    end
    
    

    result1=open([directory result1_basename]);
    result2=open([directory result2_basename]);


    if (isempty(detail_range1))
        detail_range1=result1.imagerange;
    end
    if (isempty(detail_range2))
        detail_range2=result2.imagerange;
    end


    idxs1=result1.imagerange;
    idxs2=result2.imagerange;
    figure(3)
    subplot(2,1,1);
    errorbar(result1.t(idxs1), result1.pmean(idxs1,2), result1.pstd(idxs1,2), '+b');
    hold on
    errorbar(result2.t(idxs2), result2.pmean(idxs2,2), result2.pstd(idxs2,2), '+r');
    hold off
    xlabel('beam position [µm]');
    ylabel('peak position [µm]');
    title('beam position');
    legend(result1_name, result2_name);

    subplot(2,1,2);
    errorbar(result1.t(idxs1), result1.pmean(idxs1,3), result1.pstd(idxs1,3), '+b');
    hold on
    errorbar(result2.t(idxs2), result2.pmean(idxs2,3), result2.pstd(idxs2,3), '+r');
    hold off
    xlabel('beam position [µm]');
    ylabel('1/e² peak width [µm]');
    title('1/e² beam width');

    legend(result1_name, result2_name);

    saveas(3, [directory output_basename '_detail.pdf']);
    saveas(3, [directory output_basename '_detail.fig']);

    
   
    idxs1=detail_range1;
    idxs2=detail_range2;
    
    figure(4)
    subplot(2,2, [1 3])
    
    
    datax=result1.t(idxs1);
    datay=result1.pmean(idxs1,3)';
    ids=find((datay>=expectedWidthRange(1))&(datay<=expectedWidthRange(2)));
    p0=[ expectedWidthRange(1) 50 mean(detail_range1)];
    p1 = lsqcurvefit(@fit_gaussBeamWidth,p0,datax(ids),datay(ids))
    
    datax=result2.t(idxs2);
    datay=result2.pmean(idxs2,3)';
    ids=find((datay>=expectedWidthRange(1))&(datay<=expectedWidthRange(2)));
    p0=[expectedWidthRange(1) 50 mean(detail_range2) ];
    p2 = lsqcurvefit(@fit_gaussBeamWidth,p0,datax(ids),datay(ids))
    
    
    mi=min(result1.t(idxs1));
    ma=max(result1.t(idxs1));
    tt1=mi:((ma-mi)/1000):ma;
    mi=min(result2.t(idxs2));
    ma=max(result2.t(idxs2));
    tt2=mi:((ma-mi)/1000):ma;
    errorbar(result1.t(idxs1), result1.pmean(idxs1,3), result1.pstd(idxs1,3), '+b');
    hold on
    plot(tt1, fit_gaussBeamWidth(p1, tt1), 'c-');
    errorbar(result2.t(idxs2), result2.pmean(idxs2,3), result2.pstd(idxs2,3), '+r');
    plot(tt2, fit_gaussBeamWidth(p2, tt2), 'm-');
    hold off
    xlabel('beam position [µm]');
    ylabel('1/e² peak width [µm]');
    title('1/e² beam width');
    ylim(expectedWidthRange);
    fitparams{1}=[result1_name ':'];
    fitparams{2}=['  w0  = ' num2str(p1(1)) 'µm'];
    fitparams{3}=['  z0  = ' num2str(p1(2)) 'µm'];
    fitparams{4}=['  pos = ' num2str(p1(3)) 'µm'];
    fitparams{5}=[result2_name ':'];
    fitparams{6}=['  w0  = ' num2str(p2(1)) 'µm'];
    fitparams{7}=['  z0  = ' num2str(p2(2)) 'µm'];
    fitparams{8}=['  pos = ' num2str(p2(3)) 'µm'];
    text(0.8, 0.8, fitparams, 'Units', 'normalized')
    legend(result1_name, [result1_name ', fit'], result2_name, [result2_name ', fit'], 'Location', 'SouthOutside');
    
    subplot(2,2,2)
    errorbar(result1.t(idxs1), result1.pmean(idxs1,2), result1.pstd(idxs1,2), '+b');
    hold on
    errorbar(result2.t(idxs2), result2.pmean(idxs2,2), result2.pstd(idxs2,2), '+r');
    hold off
    legend(result1_name, result2_name, 'Location', 'SouthEast');
    xlabel('beam position [µm]');
    ylabel('peak position [µm]');
    title('peak position');
    
    subplot(2,2,4)
    idxs=max(min(idxs1), min(idxs2)):min(max(idxs1), max(idxs2));
    plot(result1.t(idxs), result1.pmean(idxs,2)-result2.pmean(idxs,2), '+b');
    legend(result1_name, result2_name, 'Location', 'SouthEast');
    xlabel('beam position [µm]');
    ylabel('distance peak positions [µm]');
    title('peak position distances');
    l=ylim;
    if l(1)<-0.2
        l(1)=-0.2;
    end
    if l(2)>0.2
        l(2)=0.2;
    end
    ylim(l);
    

    saveas(4, [directory output_basename '_detaily.pdf']);
    saveas(4, [directory output_basename '_detaily.fig']);


clear

directory='.\2012_06_13\';
filename='lightsheet_zstack_000.lightpath1.tif';
readmode='read_one_file'; % 'read_file_sequence'

result_basename=filename;
mode='fitavg';%'fitall';
initparammode='takedefault'; %'takelast'
imagerange=1:100;

expectedWidthRange=[1 3];


if (strcmp(readmode, 'read_one_file'))
    [f,t]=load_tiff_sequence(result_basename, directory);%, directory);
elseif (strcmp(readmode, 'read_one_file'))
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

pixsize=[0.4 0.4 1];

[w h N]=size(f);
nparams=4;

pmean(1:N,1:nparams)=0;
pstd(1:N,1:nparams)=0;
pp(1:N,1:h,1:nparams)=0;
for idx=imagerange
    if (strcmp(mode, 'fitall')) 
        for hh=1:h
            datay=f(:,hh,idx);
            datax=pixsize(1).*(1:w)';
            [m mi]=max(datay);
            p0=[m, datax(mi(1)), 100, min(datay)];
            if hh>1  && strcmp(initparammode, 'takelast')
                p0= pp(idx,hh-1,:);
            end
            pp(idx,hh,:) = lsqcurvefit(@fit_gauss1Dvar,p0,datax,datay);
            
            pp(idx,hh,3)=sqrt(abs(pp(idx,hh,3)));
            
            if mod(hh,int16(h/3))==0
                fh=figure(2);
                fitx=pixsize(1).*(1:(w/1000):w)';
                plot(datax, datay);
                xlabel('position [µm]');
                ylabel('intensity [A.U.]');
                hold on;
                fity=fit_gauss1D(pp(idx,hh,:), fitx);
                plot(fitx, fity, 'r-');
                hold off;
                pause(0.01);
                saveas(2, [directory result_basename '_n' num2str(idx) '_fitr' num2str(hh) '.pdf']);
                saveas(2, [directory result_basename '_n' num2str(idx) '_fitr' num2str(hh) '.fig']);
                %close(fh);
            end
        end
        pmean(idx,:)=mean(pp(idx,:,:));
        pstd(idx,:)=std(pp(idx,:,:));
    elseif (strcmp(mode, 'fitavg')) 
        datay=sum(f(:,1:h,idx),2);
        datax=pixsize(1).*(1:w)';
        [m mi]=max(datay);
        p0=[m, datax(mi(1)), 100, min(datay)];
        if idx>imagerange(1) && strcmp(initparammode, 'takelast')
            p0= pp(idx-1,1,:);
        end
        
        options = optimset('lsqcurvefit');
        options = optimset(options, 'MaxFunEvals',3000);
        options = optimset(options, 'MaxIter',1000);
        pp(idx,1,:) = lsqcurvefit(@fit_gauss1Dvar,p0,datax,datay,[],[],options);
        pp(idx,1,3)=sqrt(abs(pp(idx,1,3)));

        fh=figure(2);
        fitx=pixsize(1).*(1:(w/1000):w)';
        plot(datax, datay);
        xlabel('position [µm]');
        ylabel('intensity [A.U.]');
        hold on;
        fity=fit_gauss1D(pp(idx,1,:), fitx);
        plot(fitx, fity, 'r-');
        hold off;
        pause(0.01);
        saveas(2, [directory result_basename '_n' num2str(idx) '_fitavg.pdf']);
        saveas(2, [directory result_basename '_n' num2str(idx) '_fitavg.fig']);
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
    saveas(1, [directory result_basename '.pdf']);
    saveas(1, [directory result_basename '.fig']);
end

save([directory result_basename '.mat']);

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
saveas(1, [directory result_basename '.pdf']);
saveas(1, [directory result_basename '.fig']);

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
    saveas(3, [directory result_basename '_detail.pdf']);
    saveas(3, [directory result_basename '_detail.fig']);

    
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
    saveas(4, [directory result_basename '_detaily.pdf']);
    saveas(4, [directory result_basename '_detaily.fig']);

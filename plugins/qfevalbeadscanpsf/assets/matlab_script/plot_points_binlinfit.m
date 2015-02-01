function plot_points_binlinfit( dX, dY, nbins, q, xlab, ylab, varargin )
%plot_points_binlinfit(dataX, dataY, nbins, quantile, xlab, ylab) plots the data in dataX and dataY into the current figure together with a binned version and a linear fit
%   plots the data in the vectors  dataX, dataY into the current figure,
%   adds a binned plot of the data (with errorbars and nbins bins equally
%   spaced over the datarange. If quantile is >0, the lower and upper
%   quantile if the y-data is filtered out, before plotting (i.e. if
%   quantile is 0.1, only the data withing the 10%-90% quantile range is
%   used!). the parameters xlab and ylab set the plot labels.

    fontsize=10;

    if (nargin>6)
        cnt=1;
        for n=7:2:nargin
            name=varargin{cnt};
            data=varargin{cnt+1};
            if (strcmp(name, 'FontSize'))
                fontsize=data;
            end
            cnt=cnt+2;
        end
    end

    xrange=[min(dX) max(dX)];
    if (q>0)
        yrange=[quantile(dY, q) quantile(dY, 1-q)];
    else
        yrange=[min(dY) max(dY)];
    end

    binwidth=(xrange(2)-xrange(1))/nbins;
    bins=xrange(1):binwidth:xrange(2);

    idx=(yrange(1)<=dY)&(yrange(2)>=dY);
    dX=dX(idx);
    dY=dY(idx);

    polyres=polyfit(dX, dY, 1);
    x=linspace(xrange(1), xrange(2), 50);

    dXBm=zeros(length(bins),1);
    dYBm=zeros(length(bins),1);
    dYBs=zeros(length(bins),1);
    cnt=1;
    for b=bins
        dXBm(cnt)=b+0.5*binwidth;
        dYBm(cnt)=mean(dY(dX>b & dX<b+binwidth));
        dYBs(cnt)=std(dY(dX>b & dX<b+binwidth));
        cnt=cnt+1;
    end

    plot(dX, dY, 'g.');
    dohold=~ishold();
    if (dohold)
        hold on
    end
    errorbar(dXBm, dYBm, dYBs, 'b*-');
    plot(x, polyval(polyres, x), 'r-');
    if (dohold)
        hold off
    end
    set(gca, 'FontSize', fontsize);
    leg=legend('data', ['binned bw=' num2str(binwidth)], ['lin. fit f(x)= ' num2str(polyres(1)) '*x + ' num2str(polyres(2))  ]);
    set(leg, 'FontSize', fontsize);
    xlabel(xlab, 'FontSize', fontsize);
    ylabel(ylab, 'FontSize', fontsize);
    %set(gcf, 'FontSize', fontsize);


end

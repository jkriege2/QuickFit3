%% Burst Search data import
clear;
filenames={};
% filename='E:\Documents and Settings\sebastian\My Documents\programming\QtSpielwiese\out.txt';
% filename='E:\Documents and Settings\sebastian\My Documents\programming\QtSpielwiese\APBS_sliding.txt';
% filename='E:\Documents and Settings\sebastian\My Documents\programming\QtSpielwiese\AllPhotonsBurstSearch.txt';
% filename='E:\Documents and Settings\sebastian\My Documents\programming\QtSpielwiese\DualChannelBurstSearch.txt';
directory='E:/Documents and Settings/sebastian/My Documents/Data/2013-05-02 FRET Probe/';
filename=strcat(directory,'FRET50pM10min_1zu10_AlexData.asc');
filenames{end+1}=filename;

data=importdata(filename);
start=data.data(:,1);
startTime=data.data(:,2);
burstSize=data.data(:,3);
duration=data.data(:,4); % in ms
burstRate=burstSize./duration*1e3;
meanBurstRate=mean(burstRate)
meanInverseBurstRate=1/mean(1./burstRate)
plot(startTime,burstSize,'x')
xlabel('start time')
ylabel('burst size')
clear data;

%% plots
h=1;
% plot burst size distribution (BSD)
% expect exponential distribution
h=nextfigure(h);
plotBSD(burstSize);
% xlim([0,500])

% plot burst duration distribution (BDD)
% expect approx log normal distribution
h=nextfigure(h);
plotBDD(duration);

h=nextfigure(h);
lagtime=computeLagTime(startTime);
hist(lagtime,2000)
title('Distribution of time between bursts')
xlabel('time between bursts in ms')
ylabel('frequency')

h=nextfigure(h);
lagtime=lagtime(lagtime<0.03);
hist(lagtime,200)
title('Distribution of time between bursts')
xlabel('time between bursts in ms')
ylabel('frequency')
% set(gca, 'Yscale', 'log')

h=nextfigure(h);
plotSizeDur(burstSize,duration);

h=nextfigure(h);
duration_slice=duration(burstSize<200);
burstSize_slice=burstSize(burstSize<200);
plotSizeDur(burstSize_slice,duration_slice);

% compute mean and error
range=min(burstSize):max(burstSize);
durationmean=zeros(size(range));
durationstd=zeros(size(range));
for i=1:length(range)
        durationmean(i)=mean(duration(burstSize==range(i)));
        durationstd(i)=std(duration(burstSize==range(i)));
end

h=nextfigure(h);
plotSizeDur(range,durationmean);

h=nextfigure(h);
duration_slice=durationmean(range<200);
burstSize_slice=range(range<200);
plotSizeDur(burstSize_slice,duration_slice);

h=nextfigure(h);
hist(burstRate,200);
title('Burst Rate Distribution')
xlabel('rate in kHz')
ylabel('frequency')

%% save plots
% format is name_M_L_T_h.png
% M,L,T are SBS parameters. T in us. name is the t3r filename and h the
% plot number
M=6;
L=25;
T=200;
name='testBurstSearchPerformance';
dirname = strcat('E:\Documents and Settings\sebastian\My Documents\MATLAB\Burstanalyse\',name);
mkdir(dirname)
for i=1:h
    filename=sprintf('%s\\%s_M%.2i_L%.2i_T%.3i_%.2i.png',dirname,name,M,L,T,i);
    saveas(figure(i), filename)
end

for i=1:length(filenames)
    copyfile(filenames{i},dirname);
end


cd('E:\Documents and Settings\sebastian\My Documents\MATLAB')
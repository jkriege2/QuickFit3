%% Burst Search Comparison
% filename=strcat(directory,'compareBurstSearch_M_Sliding_Combined.txt');
% filename=strcat(directory,'compareBurstSearch_MinBurstSize_Sliding_Combined.txt');

clear;
h=0;
directory='D:\isbaner\Documents\B040-SVN\FCSTOOLS\trunk\ALEXEval';
str={};

% data import
str{end+1}='T';
unitstr='in us';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_Sliding_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1); nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-');
ylim([-0.1 1.1])
legend('PR','SR') 
title(strcat('Burst Search Performance: ',str{end}))
xlabel(strcat(str{end},unitstr))


% data import
str{end+1}='M';
unitstr=' [#neighbours]';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_Sliding_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1); nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))



% data import
str{end+1}='MinBurstSize';
unitstr=' [#photons]';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_Sliding_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1); nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))

% data import
str{end+1}='MinBurstrate';
unitstr=' in kHz';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_Sliding_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1)*1e-3; nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))


% data import
str{end+1}='MaxBurstrate';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_Sliding_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1)*1e-3; nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))


% data import
str{end+1}='MaxIPT';
unitstr=' in us';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_LeeFilter_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1); nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;
x=50:50:450; % FIXME

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))



% data import
str{end+1}='LeeWinSize';
unitstr=' [#photons]';
filename=strcat(directory,'/compareBurstSearch_',str{end},'_LeeFilter_Combined.txt');
data=importdata(filename,'\t',2);
x=data.data(:,1); nBursts=data.data(:,2);
burstSize=data.data(:,3); duration=data.data(:,4); % in ms
pr=data.data(:,5); sr=data.data(:,6); burstRate=burstSize./duration*1e3;

% plots
burstSize(burstSize<0)=0; % when the burstVector is empty, c++ writes a large negative int to file
h=nextfigure(h);
subplot(4,1,1); plot(x,nBursts,'.-','MarkerSize',5);
title(strcat('Burst Search Performance: ',str{end}))
ylabel('#bursts')
subplot(4,1,2); plot(x,burstSize,'.-','MarkerSize',5);
ylabel('burst size')
subplot(4,1,3); plot(x,burstRate,'.-','MarkerSize',5);
ylabel('burst rate')
subplot(4,1,4); plot(x,duration,'.-','MarkerSize',5);
ylabel('burst duration')
xlabel(strcat(str{end},unitstr))

h=nextfigure(h);
plot(x,pr,'-',x,sr,'-'); ylim([-0.1 1.1]) 
legend('PR','SR')  
title(strcat('Burst Search Performance: ',str{end})) 
xlabel(strcat(str{end},unitstr))


%% save plots
% format is name_h
% name is the t3r filename and h the
% plot number
name='compareBurstSearch';
dirname=strcat('D:\isbaner\Documents\MATLAB','/',name);
mkdir(dirname);


for i=1:h
    filename=sprintf('%s\\%s_%.2i.png',dirname,str{ceil(i/2)},mod(i,2));
    saveas(figure(i), filename)
end

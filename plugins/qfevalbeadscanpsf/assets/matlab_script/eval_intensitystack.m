function [ statres ] = eval_intensitystack( PLaser, framesPerPower, data, rangeX, rangeY, excludePercentileUp, excludePercentileLow)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

sd=size(data)

exclude=false(sd(1),sd(2));
if (excludePercentileUp>0)
    d=data(:,:,1);
    val=quantile(d(:),1-excludePercentileUp);
    exclude(:)=exclude(:)|(d(:)>val);
end

if (excludePercentileLow>0)
    d=data(:,:,1);
    val=quantile(d(:),excludePercentileLow);
    exclude(:)=exclude(:)|(d(:)<val);
end

expix=true(sd(1),sd(2));
expix(rangeX,rangeY)=false;

exclude=exclude|expix;

frame=1;
cnt=1;
statres=zeros(length(PLaser),3);
statres(:,1)=PLaser;
for p=PLaser
    frames=frame:(frame+framesPerPower-1);
    d=[];
    for f=frames
        dd=data(:,:,f);
        d=[d dd(~exclude)];
    end
    %data(~exclude,frames);    
    statres(cnt,2)=mean(d(:));
    statres(cnt,3)=std(d(:));
    statres(cnt,4)=median(d(:));
    statres(cnt,5)=quantile(d(:), 0.25);
    statres(cnt,6)=quantile(d(:), 0.75);
    frame=frame+framesPerPower;
    cnt=cnt+1;
end

end


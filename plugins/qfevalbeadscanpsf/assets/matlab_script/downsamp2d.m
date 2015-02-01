function M=downsamp2d(M,bindims)
%DOWNSAMP2D - simple tool for 2D downsampling
%
%  M=downsamp2d(M,bindims)
%
%in:
%
% M: a matrix
% bindims: a vector [p,q] specifying pxq downsampling
%
%out:
%
% M: the downsized matrix
%
% taken from: https://groups.google.com/forum/?fromgroups#!topic/comp.soft-sys.matlab/hcJeflz9NYk[1-25]

p=bindims(1); q=bindims(2);
[m,n]=size(M); %M is the original matrix

M=sum(  reshape(M,p,[]) ,1 );
M=reshape(M,m/p,[]).'; %Note transpose

M=sum( reshape(M,q,[]) ,1);
M=reshape(M,n/q,[]).'; %Note transpose

M=M/(p*q);
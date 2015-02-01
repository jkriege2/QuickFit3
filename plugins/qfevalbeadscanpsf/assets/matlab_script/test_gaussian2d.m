tic

Nx=100;
Ny=250;

g=zeros(Ny,Nx);
x0=[50 50];
sigma=[50 20];
theta=5/180*pi;
offset=0.5;
noise=0.2;
amplitude=5;

fitx_pixelwidth=1;
fity_pixelwidth=0.5;
xxvec=((1:Nx)-1)*fitx_pixelwidth;
yyvec=((1:Ny)-1)*fity_pixelwidth;
[X,Y] = meshgrid(xxvec, yyvec);
g=Gaussian2Drmg( X,Y, offset, amplitude, x0, sigma, theta);


g=g+randn(Ny,Nx)*noise;
toc
figure(3)
subplot(1,3,1)
%x=(1:Nx-cshift(1))*cscale(1);
%y=(1:Ny-cshift(2))*cscale(2);
x=xxvec;
y=yyvec;
imagesc(x,y,g);
hold on
daspect([1 1 1]);
xlabel('x');
ylabel('y');
set(gca,'YDir','normal');

backval=imestimatebackground(g);

[cv, com]=imcov(g-backval, 'w', [fity_pixelwidth, fitx_pixelwidth]);%, 'cscale', cscale, 'cshift', cshift);
[iV, iD]=eig(cv);
[iD, iX]=sort(diag(iD), 'descend');
iV(iX,:)=iV(1:ndims(g),:);
niV=iV;
niV(1,:)=iV(1,:)/norm(iV(1,:));
niV(2,:)=iV(2,:)/norm(iV(2,:));
sx0=com;
ssigmas=sqrt(iD);
sangle=atan2(iV(1,2), iV(1,1));
sangle/pi*180;

plot_gaussianprops_image( backval, amplitude, sx0, ssigmas*2, sangle);
title('data + estimation');

hold off;

drawnow;
 
[ fx0, fsigmas, fangle, foffset, famplitude ]=performfit_gaussian2Dr(g, 'w', [fity_pixelwidth fitx_pixelwidth]);
disp([' x0 was [' num2str(x0) ']   fit: [' num2str(fx0) ']  ( estimation: [' num2str(sx0') '])']);
disp([' sigmas was [' num2str(sigma) ']   fit: [' num2str(fsigmas) ']  ( estimation: [' num2str(ssigmas') '])' ]);
disp([' angle was ' num2str(theta/pi*180) '° fit: ' num2str(fangle/pi*180) '°  ( estimation: [' num2str(sangle/pi*180) '°)' ]);
disp([' offset was ' num2str(offset) ' fit: ' num2str(foffset) '  ( estimation: [' num2str(backval) ')' ]);
disp([' amplitude was ' num2str(amplitude) ' fit: ' num2str(famplitude) ]);

subplot(1,3,2)
datasize=size(g);
gg=Gaussian2Drmg( X,Y, foffset, famplitude, fx0, fsigmas, fangle);
imagesc(xxvec, yyvec, gg);
daspect([1 1 1]);
xlabel('x');
ylabel('y');
set(gca,'YDir','normal');
title('fit result function plot');

figure(3)
subplot(1,3,3)
imagesc(xxvec, yyvec, g);
colormap(gray)
hold on
daspect([1 1 1]);
xlabel('x');
ylabel('y');
set(gca,'YDir','normal');

plot_gaussianprops_image( foffset, famplitude, fx0, fsigmas, fangle);
hold off
title('data + fit result');

clear all;

% vectors are [x y z] !!!
x0 = [ 2.1 2.1  2.1];
phi=0*pi/2;
theta=0*pi/2;
sigmas= [0.8 0.8 0.2];
alpha=0*pi/2;
offset=0.2;
amplitude=10;
noise=0.2;
disp(['ideal theta: ' num2str(theta/pi*180) '°']);
disp(['ideal phi: ' num2str(phi/pi*180) '°']);
disp(['ideal alpha: ' num2str(alpha/pi*180) '°']);
  
Nx=21;
Ny=21;
Nz=21;

wxy=0.2;
wz=0.2;

xx=((1:Nx)-1)*wxy;
yy=((1:Ny)-1)*wxy;
zz=((1:Nz)-1)*wz;

[X Y Z]=meshgrid(xx, yy, zz);
g=Gaussian3Drmg(X,Y,Z, offset, amplitude, x0, sigmas, theta, phi, alpha);

[ er, etheta, ephi ] = sphericalCoordinatesUnitVectors(theta, phi);
M=directionAngleRotationMatrix(er, alpha);
rtheta=M*etheta';
rphi=M*ephi';

figure(1);
subplot(2,2,1);
cla
plot_gaussianprops3D_3D( x0, sigmas, theta, phi, 0, 'faceColor',[0,0,1],'axisWidth',1,'xlim',[0 (Nx-1)*wxy],'ylim',[0 (Ny-1)*wxy],'zlim',[0 (Nz-1)*wz],'faceAlpha', 0.3 )
hold on;
plot_gaussianprops3D_3D( x0, sigmas, theta, phi, alpha, 'faceColor',[1,0,0],'axisWidth',2 )
grid on
set(gca,'YDir','normal');
set(gca,'ZDir','normal');
set(gca,'XDir','normal');
X=[ 1 x0(1)];
Y=[ 1 x0(2)];
Z=[ 1 x0(3)];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'black', 'LineWidth', 3);
X=[ 1 Nx];
Y=[ 1 1];
Z=[ 1 1];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'black', 'LineWidth', 1);
X=[ 1 1];
Y=[ 1 Ny];
Z=[ 1 1];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'black', 'LineWidth', 1);
X=[ 1 1];
Y=[ 1 1];
Z=[ 1 Nz];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'black', 'LineWidth', 1);
hold off

subplot(2,2,2);
imagesc(xx, zz, squeeze(g(round(x0(2)/wxy+1),:,:))');
colormap('Jet')
hold on
plot_gaussianprops3D_image( x0, 'xz', offset, amplitude, x0, sigmas, theta, phi, alpha, 'wxy', wxy, 'wz', wz )
hold off
xlabel('x');
ylabel('z');
set(gca,'YDir','normal');
daspect([1 wz/wxy 1]);
subplot(2,2,3);
imagesc(zz, yy, squeeze(g(:,round(x0(1)/wxy+1),:)));
hold on
plot_gaussianprops3D_image( x0, 'zy', offset, amplitude, x0, sigmas, theta, phi, alpha, 'wxy', wxy, 'wz', wz )
hold off
xlabel('z');
ylabel('y');
set(gca,'YDir','normal');
daspect([wz/wxy 1 1]);
subplot(2,2,4);
imagesc(xx, yy, squeeze(g(:,:,round(x0(3)/wz+1))));
hold on
plot_gaussianprops3D_image( x0, 'xy', offset, amplitude, x0, sigmas, theta, phi, alpha, 'wxy', wxy, 'wz', wz )
hold off
xlabel('x');
ylabel('y');
set(gca,'YDir','normal');
daspect([1 1 wz/wxy]);























g=g+randn(Ny,Nx,Nz)*noise;
[ fx0, fsigmas, ftheta, fphi, falpha, foffset, famplitude ] = performfit_gaussian3Dr( g, 'verbose', true, 'w', [wxy wxy wz] );
disp([' x0        = [' num2str(fx0) ']     (ideal: [' num2str(x0) '] )']);
disp([' sigma     = [' num2str(fsigmas) ']     (ideal: [' num2str(sigmas) '] )']);
disp([' offset    = [' num2str(foffset) ']     (ideal: [' num2str(offset) '] )']);
disp([' amplitude = [' num2str(famplitude) ']     (ideal: [' num2str(amplitude) '] )']);
disp([' theta     = [' num2str(ftheta/pi*180) ']     (ideal: [' num2str(theta/pi*180) '] )']);
disp([' phi       = [' num2str(fphi/pi*180) ']     (ideal: [' num2str(phi/pi*180) '] )']);
disp([' alpha      = [' num2str(falpha/pi*180) ']     (ideal: [' num2str(alpha/pi*180) '] )']);

figure(2);
subplot(2,2,1);
cla
plot_gaussianprops3D_3D( x0, sigmas, theta, phi, alpha, 'faceColor',[0,0,1],'axisWidth',1,'xlim',[0 (Nx-1)*wxy],'ylim',[0 (Ny-1)*wxy],'zlim',[0 (Nz-1)*wz],'faceAlpha', 0.3 )
hold on;
plot_gaussianprops3D_3D( fx0, fsigmas, ftheta, fphi, falpha, 'faceColor',[1,0,0],'axisWidth',2 )
grid on
set(gca,'YDir','normal');
set(gca,'ZDir','normal');
set(gca,'XDir','normal');
X=[ 1 x0(1)];
Y=[ 1 x0(2)];
Z=[ 1 x0(3)];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'k', 'LineWidth', 3);
X=[ 1 Nx];
Y=[ 1 1];
Z=[ 1 1];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'k', 'LineWidth', 1);
X=[ 1 1];
Y=[ 1 Ny];
Z=[ 1 1];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'k', 'LineWidth', 1);
X=[ 1 1];
Y=[ 1 1];
Z=[ 1 Nz];
line((X-1)*wxy,(Y-1)*wxy,(Z-1)*wz, 'Color', 'k', 'LineWidth', 1);
title('blue: ideal,   red: fit result');
hold off

subplot(2,2,2);
imagesc(xx, zz, squeeze(g(round(x0(2)/wxy+1),:,:))');
colormap('Gray')
hold on
plot_gaussianprops3D_image( x0, 'xz', offset, amplitude, x0, sigmas, theta, phi, alpha ,'color', 'b')
plot_gaussianprops3D_image( x0, 'xz', foffset, famplitude, fx0, fsigmas, ftheta, fphi, falpha,'color', 'r' )
hold off
xlabel('x');
ylabel('z');
set(gca,'YDir','normal');
daspect([1 1 1]);
subplot(2,2,3);
imagesc(zz, yy, squeeze(g(:,round(x0(1)/wxy+1),:)));
hold on
plot_gaussianprops3D_image( x0, 'zy', offset, amplitude, x0, sigmas, theta, phi, alpha ,'color', 'b')
plot_gaussianprops3D_image( x0, 'zy', foffset, famplitude, fx0, fsigmas, ftheta, fphi, falpha,'color', 'r' )
hold off
xlabel('z');
ylabel('y');
set(gca,'YDir','normal');
daspect([1 1 1]);
subplot(2,2,4);
imagesc(xx, yy, squeeze(g(:,:,round(x0(3)/wz+1))));
hold on
plot_gaussianprops3D_image( x0, 'xy', offset, amplitude, x0, sigmas, theta, phi, alpha ,'color', 'b')
plot_gaussianprops3D_image( x0, 'xy', foffset, famplitude, fx0, fsigmas, ftheta, fphi, falpha,'color', 'r' )
hold off
xlabel('x');
ylabel('y');
set(gca,'YDir','normal');
daspect([1 1 1]);

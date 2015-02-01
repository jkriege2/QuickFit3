clear
addpath('../');

directory='';
outputdirectory=[directory 'lightsheet_eval/'];
filename1='twolaser_alignment_xstack_laser2_561nm_1mW_000.lightpath1.tif';
filename2='twolaser_alignment_xstack_laser2_561nm_1mW_000.lightpath2.tif';
outputbasename='twolaser_alignment_xstack_laser2_561nm_1mW_000_';

result_basename1=[directory filename1];
result_basename2=[directory filename2];
mode='fitall'; % 'fitavg'  or  'fitall';
fitallmode='avg';  % 'single'  or  'avg'
initparammode='takedefault'; %  'takedefault'  or  'takelast'
imagerange=[];
widthrange=[];
pixsize=[0.400 0.400 1];
fitallsteps=32;
saveintermediates=false;

expectedWidthRange=[0.5 2.5];
expectedWidthRangeCompare=expectedWidthRange;


[f1,t1]=load_tiff_sequence(filename1, directory);%, directory);

[w1 h1 N1]=size(f1);
for cn=1:N1
    f1(:,:,cn)=f1(:,:,cn)';
end

[f2,t2]=load_tiff_sequence(filename2, directory);%, directory);

[w2 h2 N2]=size(f2);
for cn=1:N2
    f2(:,:,cn)=f2(:,:,cn)';
end


mkdir(outputdirectory);
perform_lightsheetanalysis(t1, f1, result_basename1, 'pixsize', pixsize, ...
                           'expectedWidthRange', expectedWidthRange, 'imagerange', imagerange, ...
                           'initparammode', initparammode, 'mode', mode, 'fitallsteps', fitallsteps, ...
                           'fitallmode', fitallmode, 'widthrange', widthrange, 'outputdirectory', outputdirectory, ...
                           'saveintermediates', saveintermediates);

perform_lightsheetanalysis(t2, f2, result_basename2, 'pixsize', pixsize, ...
                           'expectedWidthRange', expectedWidthRange, 'imagerange', imagerange, ...
                           'initparammode', initparammode, 'mode', mode, 'fitallsteps', fitallsteps, ...
                           'fitallmode', fitallmode, 'widthrange', widthrange, 'outputdirectory', outputdirectory, ...
                           'saveintermediates', saveintermediates);


perform_lightsheet_analysis_plot2results(directory, outputbasename, ...
                           [outputdirectory result_basename1 '.mat'], '491nm', ...
                           [outputdirectory result_basename2 '.mat'], '561nm', ...
                           'outputdirectory', outputdirectory, ...
                           'detail_range1', imagerange, 'detail_range2', imagerange, ...
                           'expectedWidthRange', expectedWidthRangeCompare);

function [ frame ] = read_CHSPAD_binary( filename, repeats )

header=16;
width=512;
height=128;

frame=zeros(width,height,repeats);

fid=fopen(filename);
for r=1:repeats
    fread(fid,header, 'uint8');
    frame(:,:,r)=reshape(fread(fid,width*height,'uint16'),height,width)';
end

end


function [ res ] = mat2strformated( X, rownames, colnames, colwidth )

[sX,sY]=size(X);
res=cell(sX,1);
coffset=0;
roffset=0;
if  length(colnames)>0
    roffset=1;
end
if length(rownames)>0
    coffset=colwidth;
    for row=1:sX
        line='';
        if length(rownames)>0
            line=sprintf(['%' num2str(colwidth) 's'], '');
        end
        for c=1:sY
            cn='';
            if c<=length(colnames)
                cn=colnames{c};
            end
            line=[line ' ' sprintf(['%' num2str(colwidth) 's'], cn)]; 
        end
        res{1}=line;
    end
end

for row=1:sX
    line='';
    if length(rownames)>0
        rn='';
        if row<=length(rownames)
            rn=rownames{row};
        end
        line=sprintf(['%' num2str(colwidth) 's'], rn);
    end
    for c=1:sY
        line=[line ' ' sprintf(['%' num2str(colwidth) 'f'], X(row,c))]; 
    end
    res{roffset+row}=line;
end

end


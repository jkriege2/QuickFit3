fn="/home/kriegerj/temperaturegradient.txt";
dev="meas_b040resheater";
devid=0;
params=new Array("TEMP0", "TEMP1");
delay=5*60000;
minidelay=2000;
repeats=60;
stage1=0;
delta1=-2000;
start1=500;
end1=start1+7*delta1;
stage2=1;
delta2=-2000;
start2=8400;
end2=start2+8*delta2;


instrument.MDConnect(dev, devid);
cnt=instrument.MDGetParamCount(dev, devid);

tools.createTextFile(fn);
tools.appendTextFile(fn, "# x[micrometer], \ty[micrometer], \tx_measured[micrometer], \ty_measured[micrometer], \tz_measured[micrometer]");
for (var k=0; k<params.length; k++) {
    tools.appendTextFile(fn, ", \tavg("+params[k]+")");
    tools.appendTextFile(fn, ", \tstd("+params[k]+")");
}
tools.appendTextFile(fn, ", \ttime_mid [s]");
tools.appendTextFile(fn, ", \ttime_meas_start [s]");
tools.appendTextFile(fn, ", \ttime_meas_end [s]");
tools.appendTextFile(fn, "\n");


var d=new Date();
var y=start2;
while (y!=end2) {
  var x=start1;
  while (x!=end1) {
    tools.logText("moving stages to stage["+stage1+", "+stage2+"] = ("+x+" / "+y+")");
    instrument.moveStageAbs(stage1, x);
    instrument.moveStageAbs(stage2, y);
    tools.logText(" ... new pos: ("+instrument.getStagePos(stage1)+" / "+instrument.getStagePos(stage2)+")\n");
    tools.sleepMS(delay);
    sum=new Array();
    sum2=new Array();
    count=new Array();    
    for (var k=0; k<params.length; k++) {
      sum[k]=0;
      sum2[k]=0;
      count[k]=0;
    }
    var d2=new Date();
    for (var i=0; i<repeats; i++) {
      for (var k=0; k<params.length; k++) {
          var value=instrument.MDGetN(dev, devid, params[k]);
          sum[k]=sum[k]+value;
          sum2[k]=sum2[k]+value*value;
          count[k]=count[k]+1;
      }
      tools.sleepMS(minidelay);
    }
    var d3=new Date();
    tools.appendTextFile(fn, x);
    tools.appendTextFile(fn, ", \t"+y);
    tools.appendTextFile(fn, ", \t"+instrument.getStagePos("x"));
    tools.appendTextFile(fn, ", \t"+instrument.getStagePos("y"));
    tools.appendTextFile(fn, ", \t"+instrument.getStagePos("z"));
    for (var k=0; k<params.length; k++) {
        var avg=sum[k]/count[k];
        var stabw=Math.sqrt( ( sum2[k] - sum[k]*sum[k]/count[k])/(count[k]-1));
        tools.logText("    measured "+params[k]+" = ("+avg+" +/- "+stabw+")°C\n");
        tools.appendTextFile(fn, ", \t"+avg);
        tools.appendTextFile(fn, ", \t"+stabw);
    }
    tools.appendTextFile(fn, ", \t"+(((d2-d)+(d3-d))/2000));
    tools.appendTextFile(fn, ", \t"+((d2-d)/1000));
    tools.appendTextFile(fn, ", \t"+((d3-d)/1000));
    tools.appendTextFile(fn, "\n");
    x=x+delta1;
  }
  y=y+delta2;
}

instrument.MDSetN(dev, devid, "HEATING_STATE", 0);
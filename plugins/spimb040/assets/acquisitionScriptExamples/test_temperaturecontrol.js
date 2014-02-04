fn="/home/kriegerj/temperature_log.txt";
dev="meas_b040resheater";
devid=0;
params=new Array("TEMP0", "TEMP1", "OUTPUT", "SET_TEMP");
delay=250;
repeats=60*60/(delay/1000);

tools.createTextFile(fn);
tools.appendTextFile(fn, "# time [s]");
for (var i = 0; i < params.length; ++i) {
    tools.appendTextFile(fn, ", \t"+params[i]);
}
tools.appendTextFile(fn, "\n");

var d=new Date();
instrument.MDConnect(dev, devid);
cnt=instrument.MDGetParamCount(dev, devid);
for (var i=0; i<repeats; i++) {
    var d2=new Date();
    tools.logText("repeat "+i+"/"+repeats+": "+d2+"\n");
    tools.appendTextFile(fn, (d2-d)/1000);
  for (var j = 0; j < params.length; ++j) {
    for (var k=0; k<instrument.MDGetParamCount(dev, devid); k++) {
      if (instrument.MDGetParamName(dev, devid, k)==params[j]) {
        tools.appendTextFile(fn, ", \t"+instrument.MDGet(dev, devid, k));
          }
       }
  }
    tools.appendTextFile(fn, "\n");
    tools.sleepMS(delay);
}
instrument.MDDisconnect(dev, devid);

tools.appendTextFile(fn, "\n");
fn="/home/kriegerj/temperature_P10_I1_D0_IMAX200_on_off_log.txt";
dev="meas_b040resheater";
devid=0;
params=new Array("TEMP0", "TEMP1", "OUTPUT", "SET_TEMP");
delay=1000;
repeats=Math.ceil(30*60/(delay/1000));
heatingon_after=Math.ceil(2*60/(delay/1000));
heatingoff_after=Math.ceil(2*repeats/3.0);

instrument.MDConnect(dev, devid);
cnt=instrument.MDGetParamCount(dev, devid);
instrument.MDSetN(dev, devid, "HEATING_STATE", 0);
tools.logText("\n\n!!! SWITCHING OFF HEATING !!!\n\n");

tools.createTextFile(fn);
tools.appendTextFile(fn, "# time [s]");
for (var k=0; k<cnt; k++) {
    tools.appendTextFile(fn, ", \t"+instrument.MDGetParamName(dev, devid, k));
}
tools.appendTextFile(fn, "\n");


var d=new Date();
for (var i=0; i<repeats; i++) {
    var d2=new Date();
    tools.logText("repeat "+i+"/"+repeats+": "+d2+"\n");
    tools.appendTextFile(fn, (d2-d)/1000);
    for (var k=0; k<cnt; k++) {
        tools.appendTextFile(fn, ", \t"+instrument.MDGet(dev, devid, k));
    }
    tools.appendTextFile(fn, "\n");
    tools.sleepMS(delay);
    if (i==heatingoff_after) {
        instrument.MDSetN(dev, devid, "HEATING_STATE", 0);
        tools.logText("\n\n!!! SWITCHING OFF HEATING !!!\n\n");
    }
    if (i==heatingon_after) {
        instrument.MDSetN(dev, devid, "HEATING_STATE", 1);
        tools.logText("\n\n!!! SWITCHING ON HEATING !!!\n\n");
    }
}
instrument.MDDisconnect(dev, devid);

tools.appendTextFile(fn, "\n");
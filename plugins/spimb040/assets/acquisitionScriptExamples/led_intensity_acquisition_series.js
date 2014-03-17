
params=new Array(1,2,5,7.5,10,15,20,25);
delay=2000;

for (var i=0; i<params.length; i++) {
    tools.logText("setting laser to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(  delay);
    tools.logText("running acquisition at " + params[i]+"mA\n");
    instrument.setAcqDescCurrentSample("LEDIntensity_"+params[i]+"mA");
    acquisition.doAcquisition();
}

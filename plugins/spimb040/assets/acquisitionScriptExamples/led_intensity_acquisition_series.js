
params=new Array(0,1,5,10,20);
delay=2000;

for (var i=0; i<params.length; i++) {
    tools.logText("setting laser to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(  delay);
    tools.logText("running acquisition at " + params[i]+"mA\n");
    instrument.setAcqDescCurrentSample("LED_"+params[i]+"mA_24V_800mVQ");
    acquisition.doAcquisition();
}

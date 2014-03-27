
params=new Array(5, 10, 15);
delay=5000;

for (var i=0; i<params.length; i++) {
    tools.logText("setting laser to " + params[i]+"mW\n");
    instrument.setLaserIntensity(0, 0, params[i]);
    tools.sleepMS(  delay);
    instrument.setAcqDescCurrentSample("TetraSpec_LP" + params[i] +"mW__");
    tools.logText("running acquisition at " + params[i]+"mW\n");
    acquisition.doAcquisition();
}

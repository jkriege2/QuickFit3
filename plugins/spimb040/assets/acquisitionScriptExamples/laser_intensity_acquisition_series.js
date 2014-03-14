
params=new Array(5, 10, 15, 20, 25, 30, 35, 40,45, 50, 55, 60);
delay=5000;

for (var i=0; i<params.length; i++) {
    tools.logText("setting laser to " + params[i]+"mW\n");
    instrument.setLaserIntensity(0, 0, params[i]);
  tools.sleepMS(  delay);
    tools.logText("running acquisition at " + params[i]+"mW\n");
    acquisition.doAcquisition();
}

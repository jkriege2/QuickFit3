params_led_brightness=new Array(5,10,20,40,60);
acq_settings=["EM100 128x20", "N&B EM500 128x20, 470us", "N&B EM500 128x20, 100us"];
names=["SPIMFCS", "NANDB_470", "NANDB_100"];
samplename="TetraSpec";

tools.setStatus("<h1>measuring</h1>\n");
for (var i=0; i<params_led_brightness.length; i++) {
  instrument.setLaserIntensity(0, 0, params_led_brightness[i]);
  tools.sleepMS(1000);
  for (var j=0; j<acq_settings.length; j++) {
    instrument.setAcqDescPrefix(names[j]+"_"+samplename+"_;");
    acquisition.setAcquisitionConfigIndex(0, acq_settings[j]);
    acquisition.doAcquisition();
  }
}  

params_led_brightness=new Array(20,10,1,0);
delay=2000;
for (var i=0; i<params_led_brightness.length; i++) {
  acquisition.clearAcquisitionCamera2Settings();
  tools.logText("setting LED to " + params_led_brightness[i]+"\n");
  instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
  tools.sleepMS(delay);
  instrument.setAcqDescCurrentSample("LED24V_600mVQ_" + params_led_brightness[i] + "mA");
  acquisition.doAcquisition();
}  
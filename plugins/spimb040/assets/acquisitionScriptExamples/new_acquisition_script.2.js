
params_gating_length=new Array(55,110,220,440);
params_led_brightness=new Array(20,10,1,0);
delay=2000;
for(var k=0;k<params_gating_length.length;k++){
  for (var j=0; j<2; j++) {
    for (var i=0; i<params_led_brightness.length; i++) {
      acquisition.clearAcquisitionCamera2Settings();
//      tools.logText("setting LED to " + params_led_brightness[i]+"\n");
      instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
      comment="";
      if(j==1){
        acquisition.setAcquisitionCamera2SettingN("GATING_RND", true);
        comment="_RND";
      }
      acquisition.setAcquisitionCamera2SettingN("GATING_LENGTH", params_gating_length[k]);
      tools.sleepMS(delay);
      instrument.setAcqDescCurrentSample("LED_24V_" + params_led_brightness[i] + "mA_L" + params_gating_length[k] + comment);
      acquisition.doAcquisition();
    }
  }
}  
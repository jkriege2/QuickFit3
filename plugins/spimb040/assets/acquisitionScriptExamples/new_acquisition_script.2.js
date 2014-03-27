
//params=new Array(1.0,2.0,5.0,8.0,10.0,15.0,20.0,25.0);
params=new Array(20,10,0);
delay=2000;
for (var j=0; j<2; j++) {
  for (var i=0; i<params.length; i++) {
    tools.logText("setting LED to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(delay);
    tools.logText("running acquisition at " + params[i]+"mA\n");
    comment="";
    if(j==1){
      acquisition.setAcquisitionCamera2SettingN("GATING_RND", true);
   comment="_RND"
  }
    instrument.setAcqDescCurrentSample("LED_24V_"+params[i]+"mA"+comment);
    acquisition.doAcquisition();
  }
}

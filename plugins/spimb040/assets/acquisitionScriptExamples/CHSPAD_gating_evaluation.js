tools.showMessageBox("Hello World!"+acquisition.getPreviewConfigs(1));

params_led_brightness=new Array(5,1,0);
  for (var i=0; i<params_led_brightness.length; i++) {
    instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
    instrument.setAcqDescCurrentSample("C21_24VOP_1100mVQ_2VT_LED"+params_led_brightness[i]+"mA");
    acquisition.setAcquisitionConfig(1,"gating_test_01_ROI");
    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_02");
//    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_03");
//    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_04");
//    acquisition.doAcquisition();
    acquisition.setAcquisitionConfig(1,"gating_test_05_ROI");
    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_06");
//    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_07");
//    acquisition.doAcquisition();
    acquisition.setAcquisitionConfig(1,"gating_test_08_ROI");
    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_09");
//    acquisition.doAcquisition();
//    acquisition.setAcquisitionConfig(1,"gating_test_10");
//    acquisition.doAcquisition();
  }
  
  


//19.5,19.6,19.7,19.8,19.9,20.0,20.1,20.2,20.3,20.4,20.5,20.6,20.7,20.8,20.9,21.0,21.1,21.2,21.3,21.4,21.6,21.8,22,22.2,22.4,22.6,22.8,23,23.2,23.4,23.6,23.8,24,24.2,24.4,24.6,24.8,25
/*params_VQ=new Array(19.3,19.4);
params_led_brightness=new Array(30,10,5,2,1,0);
tools.sleepMS(20000);
for (var j=0; j<params_VQ.length; j++) {
  tools.setStatus("<h1>SET VQ to " + params_VQ[j]+"</h1>\n");
  tools.sleepMS(20000);
  tools.setStatus("<h1>measuring</h1>\n");
  for (var i=0; i<params_led_brightness.length; i++) {
    acquisition.clearAcquisitionCamera2Settings();
    instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
    tools.sleepMS(1000);
    instrument.setAcqDescCurrentSample("UL_LED"+params_led_brightness[i]+"mA_"+params_VQ[j]*1000+"mV_0mVQ_");
    acquisition.acquirePreviewFrames(1,5, "%prefix%_chspad_%counter%");
    acquisition.acquirePreviewFrames(0, 3, "%prefix%_emccd_%counter%");
  }  
}*/
//19.5,19.6,19.7,19.8,19.9,20.0,20.1,20.2,20.3,20.4,20.5,20.6,20.7,20.8,20.9,21.0,21.1,21.2,21.3,21.4,21.6,21.8,22,22.2,22.4,22.6,22.8,23,23.2,23.4,23.6,23.8,24,24.2,24.4,24.6,24.8,25
params_led_brightness=new Array(0,1,2,3,4,5,6,7,8,9,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000);
tools.setStatus("<h1>measuring</h1>\n");
for (var i=0; i<params_led_brightness.length; i++) {
    acquisition.clearAcquisitionCamera2Settings();
    instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
    tools.sleepMS(1000);
    instrument.setAcqDescCurrentSample("SNR_LED_"+params_led_brightness[i]+"mA__");
    acquisition.acquirePreviewFrames(1,25, "%prefix%_chspad_%counter%");
}  

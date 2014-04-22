
params=new Array(0,1,5,10,15,20,30,40,50, 0);
emgain=new Array(0,5,10,50,100,200,300);
exposures=new Array(1000,10000,100000);
delay=2000;
frames=20;

for (var i=0; i<params.length; i++) {
    tools.logText("setting LED to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(  delay);
    tools.logText("running acquisition at " + params[i]+"mA\n");
    instrument.setAcqDescCurrentSample("LED_"+params[i]+"mA_");
    for (var j=0; j<params.length; j++) {
        for (var k=0; k<exposures.length; k++) {
            acquisition.setPreviewParam("cam_andor/emgain_enabled",  emgain[j]>0);
            acquisition.setPreviewParam("cam_andor/emgain", emgain[j]);
            acquisition.setPreviewParam("cam_andor/exposure_time", exposures[k]);
            acquisition.acquirePreviewFramesWithParams(0, frames, "%prefix%_emgain"+emgain[j]+"_exp"+exposures[k]+"_%counter%");
        }
    }
    acquisition.clearPreviewParams();
    acquisition.doAcquisition();
    acquisition.clearPreviewParams();
}

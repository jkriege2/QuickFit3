
params=new Array(0,1,2,3,4,5,7,10,12,15,17,20,25,30,35,40,45,50,55,60);
emgain=new Array(0,5,10,100);
exposures=new Array(1000,10000,100000);
delay=2000;
frames=500;

for (var i=0; i<params.length; i++) {
    tools.logText("setting LED to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(delay);    
    tools.logText("running acquisition at " + params[i]+"mA\n");
    instrument.setAcqDescCurrentSample("LED_"+params[i]+"mA_");
    for (var j=0; j<params.length; j++) {
        for (var k=0; k<exposures.length; k++) {
            acquisition.setAcquisitionCamera1Setting("frames",  frames);
            acquisition.setAcquisitionCamera1Setting("emgain", emgain[j]);
            acquisition.setAcquisitionCamera1Setting("exposuretime", exposures[k]);
			instrument.setAcqDescCurrentSample("LED" + params[i] + "mA__emgain" + emgain[j] + "mA_exp" + exposures[k] + "__" );
			acquisition.doAcquisition();
        }
    }
}

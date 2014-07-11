
params=new Array(0,1,2,3,4,5,7,10,12,15,17,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,110,120,130,14,150,160,170,180,190,200,250,300,400,500,600,700,800,900,1000);
//exposures=new Array(10000,1000);
exposure=10000;
delay=2000;
frames=1000;
emgain=new Array(0,5,10,100);

for (var i=0; i<params.length; i++) {
    tools.logText("setting LED to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(delay);    
    tools.logText("running acquisition at " + params[i]+"mA\n");

    for (var jj=0; jj<emgain.length; jj++) {
          tools.logText(i+":"+jj+"/"+emgain.length+"  LED" + params[i] + "mA__emgain" + emgain[jj] + "_exp" + exposure+ "__\n"); 
          acquisition.setAcquisitionCamera1Setting("frames",  frames);
          acquisition.setAcquisitionCamera1Setting("emgain", emgain[jj]);
          acquisition.setAcquisitionCamera1Setting("exposuretime", exposure);//exposures[k]);
          instrument.setAcqDescCurrentSample("LED" + params[i] + "mA__emgain" + emgain[jj] + "_exp" + exposure+ "__"); //exposures[k] + "__" );
          acquisition.doAcquisition();

    }
}

instrument.setBrightfieldIntensity(0, 0, 0);




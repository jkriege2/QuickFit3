
params=new Array(100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000);
exposures=new Array();
exposures.push(70000);
exposures.push(100000);
exposures.push(200000);
delay=1000;
frames=500;
emgain=new Array();
emgain.push(0,5);

for (var i=0; i<params.length; i++) {
    tools.logText("setting LED to " + params[i]+"\n");
    instrument.setBrightfieldIntensity(0, 0, params[i]);
    tools.sleepMS(delay);    
    tools.logText("running acquisition at " + params[i]+"mA\n");
    
    for (var je=0; je<exposures.length; je++) {
      exposure=exposures[je];
      for (var jj=0; jj<emgain.length; jj++) {
            if (emgain[jj]<=10 || (emgain[jj]<=50) || (emgain[jj]<=100 && params[i]<=500)) {
              tools.logText(i+":"+jj+"/"+emgain.length+"  LED" + params[i] + "mA__emgain" + emgain[jj] + "_exp" + exposure+ "__\n"); 
              acquisition.setAcquisitionCamera1Setting("frames",  frames);
              acquisition.setAcquisitionCamera1Setting("emgain", emgain[jj]);
              acquisition.setAcquisitionCamera1Setting("exposuretime", exposure);
              instrument.setAcqDescCurrentSample("OD0_LED" + params[i] + "mA__emgain" + emgain[jj] + "_exp" + exposure+ "__"); //exposures[k] + "__" );
              acquisition.doAcquisition();
            }  
      }
   }
}

instrument.setBrightfieldIntensity(0, 0, 0);




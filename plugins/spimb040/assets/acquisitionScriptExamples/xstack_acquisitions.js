xstart=0;
xend=1370;
xstep=150
delay=1000;

for (var i=xstart; i<xend; i=i+xstep) {
    instrument.moveStageAbs("x", i);
    tools.sleepMS(  delay);
    instrument.setAcqDescCurrentSample("DNA607bp_xscan2_"+i+"_OD0.5_LP__");
    tools.logText("running acquisition at x=" + i+"   real_x="+instrument.getStagePos("x")+"\n");
    acquisition.doAcquisition();
}

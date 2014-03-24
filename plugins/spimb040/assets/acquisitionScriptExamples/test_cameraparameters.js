dev="cam_server";
devid=0;

instrument.MDConnect(dev, devid);
tools.logText("camera parameters available: " +instrument.MDGetParamCount(dev, devid)+"\n");
tools.logText("BITDEPTH_LV= " +instrument.MDGetN(dev, devid, "BITDEPTH_LV")+"\n ... setting to 16 ...\n");
instrument.MDSetN(dev, devid, "BITDEPTH_LV", 16)
tools.logText("BITDEPTH_LV= " +instrument.MDGetN(dev, devid, "BITDEPTH_LV")+"\n");


acquisition.clearAcquisitionCamera2Settings();
instrument.setBrightfieldIntensity(0, 0, 60);
tools.sleepMS(120000);
instrument.setBrightfieldIntensity(0, 0, 0);
instrument.setAcqDescCurrentSample("UL_DCR_24V_800mVQ_");
acquisition.acquirePreviewFrames(1,100, "%prefix%_chspad_%counter%");
//acquisition.acquirePreviewFrames(0, frames, "%prefix%_emccd_%counter%");
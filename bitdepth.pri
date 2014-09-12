
win32 {
    BIT_DEPTH=$$system(.\\tools\\qf3bitdepth\\qf3bitdepth.exe)
} else {
    BIT_DEPTH=$$system(./tools/qf3bitdepth/qf3bitdepth)
}

message("COMPILE MODE: bit-depth: $$BIT_DEPTH")

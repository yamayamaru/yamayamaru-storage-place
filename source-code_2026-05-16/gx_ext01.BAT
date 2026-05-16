del gx_ext01.obj
del gx_ext01.map
del gx_ext01.exe
del gx_ext01.lst
del g-exten2.obj

masm gx_ext01,,gx_ext01;
link gx_ext01.obj,gx_ext01.exe,gx_ext01.map;

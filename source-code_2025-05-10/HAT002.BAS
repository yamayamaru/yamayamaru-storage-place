110 GOSUB 1000
120 DIM D(160)
130 FOR I = 1 TO 159
140 D(I)=100
150 NEXT
160 DR=3.14/180
170 FOR Y=-180 TO 180 STEP 6
180 FOR X=-180 TO 180 STEP 4
190 R=DR*SQR(X*X+Y*Y)
200 Z=100*COS(R)-30*COS(3*R)
210 SX=INT(80+X/3-Y/6)
220 SY=INT(40-Y/6-Z/4)
230 IF (SX<0) + (SX>=160) THEN 380
240 IF D(SX) <= SY THEN 380
250 ZZ=INT((Z+100)*0.035)+1
260 C = 0:F=0
270 IF (ZZ=1)+(ZZ=3)+(ZZ=5)+(ZZ=7) THEN C = C+4:F=1
280 IF (ZZ=2)+(ZZ=3)+(ZZ>=6) THEN C = C+1:F=1
290 IF ZZ>=4 THEN C = C+2:F=1
300 X1=INT(SX*2*1024/340):Y1=780-INT(SY*2*780/210)
310 IF F=0 GOTO 370
320 FOR J=0 TO 2
330 FOR K=0 TO 2
340 PX=X1+K:PY=Y1+J:GOSUB 1100:GOSUB 2100
350 NEXT K
360 NEXT J
370 D(SX)=SY
380 NEXT X
390 NEXT Y
400 END
1000 PRINT CHR$(&H1B);CHR$(&H0C);CHR$(&H0D);CHR$(&H0A);:RETURN
1100 REM TEKCOLOR(px)
1110 PRINT CHR$(&H1B);CHR$(&H5B);CHR$(&H33);CHR$(&H30+C);
1120 PRINT CHR$(&H6D);
1130 RETURN
2100 REM TEKDOT(px, py)
2110 PRINT CHR$(&H1D);
2120 PRINT CHR$(&H20+(PY/32 AND &H1F));CHR$(&H60+(PY AND &H1F));
2140 PRINT CHR$(&H20+(PX/32 AND &H1F));CHR$(&H40+(PX AND &H1F));
2160 PRINT CHR$(&H20+(PY/32 AND &H1F));CHR$(&H60+(PY AND &H1F));
2180 PRINT CHR$(&H20+(PX/32 AND &H1F));CHR$(&H40+(PX AND &H1F));
2190 RETURN

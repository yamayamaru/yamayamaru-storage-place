110 SCREEN 3:COLOR ,,,,2:CLS 2:CLS 1
120 DIM D(160)
130 for i = 1 to 159:d(i)=100:next i
140 dr=3.14/180
150 for y=-180 to 180 step 6
160 for x=-180 to 180 step 4
170 r=dr*sqr(x*x+y*y)
180 z=100*cos(r)-30*cos(3*r)
190 sx=int(80+x/3-y/6)
200 sy=int(40-y/6-z/4)
210 if (sx<0) + (sx>=160) then 310
220 if d(sx) <= sy then 310
230 zz=int((z+100)*0.035)+1
240 color01 = 0:f=0
250 if (zz=1)+(zz=3)+(zz=5)+(zz=7) then color01 = color01+1:f=1
260 if (zz=2)+(zz=3)+(zz>=6) then color01 = color01+2:f=1
270 if zz>=4 then color01 = color01+4:f=1
280 px=sx*2*640/320:py=sy*2*400/200
290 if f=1 then line(px,py)-(px+2,py+2),color01, bf
300 d(sx)=sy
310 next x:next y

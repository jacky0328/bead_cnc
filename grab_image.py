#!/usr/bin/python
import cv2
import numpy as np
import os

def writeim(x,y,i,img_source):
    img = img_source[y*60+15:(y+1)*60-15,x*60+15:(x+1)*60-15,:]
    cv2.imwrite(str(i)+'.png',img)
    cv2.imshow('image',img)
    cv2.waitKey(1000)


img_source = cv2.imread('./mask.png')

x = 1
y = 4
writeim(x,y,1,img_source)

x = 1
y = 0
writeim(x,y,2,img_source)

x = 1
y = 1
writeim(x,y,3,img_source)

x = 2
y = 2
writeim(x,y,4,img_source)

x = 0
y = 2
writeim(x,y,5,img_source)

x = 0
y = 0
writeim(x,y,6,img_source)

x = 0
y = 2
writeim(x,y,7,img_source)


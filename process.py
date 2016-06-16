#!/usr/bin/python
import bead_algorithm

import cv2
import numpy as np
import os
import serial # sudo apt-get install python3-serial
import sys
import time


z_num = 7;
point_num = 10
grid_w = 6
grid_h = 5
grid_pixel = 60

grid_x = grid_w * grid_pixel
grid_y = grid_h * grid_pixel

bead_screen = 11.5   # 11 mm


#pts1 = np.float32([[221,112],[442,115],[212,301],[446,302]])
#pts1 = np.float32([[223,122],[443,124],[215,308],[444,311]])
f = open('pos.txt', 'r')


pos0_x = int(f.readline())
pos0_y = int(f.readline())
pos1_x = int(f.readline())
pos1_y = int(f.readline())
pos2_x = int(f.readline())
pos2_y = int(f.readline())
pos3_x = int(f.readline())
pos3_y = int(f.readline())

f.close();

#pts1 = np.float32([[199,164],[426,169],[191,345],[424,347]])

pts1 = np.float32([[pos0_x,pos0_y],[pos1_x,pos1_y],[pos2_x,pos2_y],[pos3_x,pos3_y]])
pts2 = np.float32([[0,0],[grid_x,0],[0,grid_y],[grid_x,grid_y]])
M = cv2.getPerspectiveTransform(pts1, pts2)

ser = serial.Serial(
    port = '/dev/ttyUSB0',
    baudrate = 115200
)

ser.write("\r\n\r\n")
time.sleep(2)
ser.flushInput()

# change to relative mode
ser.write('G91\n')
grbl_out = ser.readline()
print ':' + grbl_out.strip()




mouse_index=0

def nothing(x):
    pass

def get_point(event,x,y,flags,param):
    global mouse_index
    global pos0_x,pos0_y
    global pos1_x,pos1_y
    global pos2_x,pos2_y
    global pos3_x,pos3_y
    if event == cv2.EVENT_LBUTTONDBLCLK:
         print "(x:%d,y:%d)"%(x,y)

         if(mouse_index==0):
            pos0_x = x
            pos0_y = y
         if (mouse_index==1):
            pos1_x = x
            pos1_y = y
         if (mouse_index==2):
            pos3_x = x
            pos3_y = y
         if (mouse_index==3):
            pos2_x = x
            pos2_y = y
            
         if(mouse_index==3):
            mouse_index = 0
         else:
            mouse_index+=1


cv2.namedWindow('Control Bar')

CV_CAP_PROP_BRIGHTNESS = 10
CV_CAP_PROP_CONTRAST   = 11
CV_CAP_PROP_SATURATION = 12
CV_CAP_PROP_GAIN       = 14
cap = cv2.VideoCapture(0)

Default_Brightness = cap.get(CV_CAP_PROP_BRIGHTNESS)
Default_Contrast   = cap.get(CV_CAP_PROP_CONTRAST)
Default_Saturation = cap.get(CV_CAP_PROP_SATURATION)
Default_Gain       = cap.get(CV_CAP_PROP_GAIN)

print "Brightness    : " + str(Default_Brightness)
print "Contrast      : " + str(Default_Contrast)
print "Saturation    : " + str(Default_Saturation)
print "Gain          : " + str(Default_Gain)

cv2.createTrackbar('Brightness','Control Bar',int(Default_Brightness*255),255,nothing)
cv2.createTrackbar('Contrast'  ,'Control Bar',int(Default_Contrast*255),255,nothing)
cv2.createTrackbar('Saturation','Control Bar',int(Default_Saturation*255),255,nothing)
cv2.createTrackbar('Gain'      ,'Control Bar',int(Default_Gain*255),255,nothing)


cv2.namedWindow('source image')
cv2.setMouseCallback('source image',get_point)



while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    cv2.imwrite('src.png',frame)

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    #  get information
    Brightness = float(cv2.getTrackbarPos('Brightness','Control Bar'))
    Contrast   = float(cv2.getTrackbarPos('Contrast'  ,'Control Bar'))
    Saturation = float(cv2.getTrackbarPos('Saturation','Control Bar'))
    Gain       = float(cv2.getTrackbarPos('Gain'      ,'Control Bar'))

    cap.set(CV_CAP_PROP_BRIGHTNESS,Brightness/255.0)
    cap.set(CV_CAP_PROP_CONTRAST,Contrast/255.0)
    cap.set(CV_CAP_PROP_SATURATION,Saturation/255.0)
    cap.set(CV_CAP_PROP_GAIN,Gain/255.0)

    # Display the resulting frame

    if (mouse_index==0):
        cv2.circle(frame,(pos0_x,pos0_y), 10, (255,0,0), 3)
        
    elif (mouse_index==1):
        cv2.circle(frame,(pos1_x,pos1_y), 10, (255,0,0), 3)
        
    elif (mouse_index==2):
        cv2.circle(frame,(pos3_x,pos3_y), 10, (255,0,0), 3)
        
    elif (mouse_index==3):
        cv2.circle(frame,(pos2_x,pos2_y), 10, (255,0,0), 3)
        
        
    cv2.line(frame,(pos0_x,pos0_y),(pos1_x,pos1_y),(0,0,255),3) 
    cv2.line(frame,(pos1_x,pos1_y),(pos3_x,pos3_y),(0,0,255),3)
    cv2.line(frame,(pos3_x,pos3_y),(pos2_x,pos2_y),(0,0,255),3)
    cv2.line(frame,(pos2_x,pos2_y),(pos0_x,pos0_y),(0,0,255),3)
    cv2.imshow('source image',frame)

    k = cv2.waitKey(1)
    if k == ord('q'):
        break
    elif ( k== ord('s')):
       img_mask = cv2.imread('src.png')
       result = cv2.warpPerspective(img_mask, M, (grid_x, grid_y))
       cv2.imwrite('mask.png',result)
       img_source = result
       img_source2 = result
       img_gray = cv2.cvtColor(img_source, cv2.COLOR_BGR2GRAY)
       font = cv2.FONT_HERSHEY_SIMPLEX
       arr = np.zeros(shape=(grid_h,grid_w), dtype = int)
       out_str = ''
       index =1
       for yy in range(0,grid_h,1):
          for xx in range(0,grid_w,1):
              img_src = img_gray[yy*grid_pixel:(yy+1)*grid_pixel,xx*grid_pixel:(xx+1)*grid_pixel]
              index = 1
              max_val =0;
              for i in range(1,8,1): # total 1.png ~ 8\7.png
                  template = cv2.imread(str(i) + ".png",cv2.IMREAD_GRAYSCALE)
                  res = cv2.matchTemplate(img_src,template,cv2.TM_CCOEFF_NORMED)
                  max_loc = np.amax(res)
                  #print max_loc
                  if(max_loc > max_val):
                      max_val = max_loc
                      index = i

              if(index ==7): #  7.png -> number 5 , 5.png -> number 5
                  index = 5

              cv2.putText(img_source,str(index),(xx*grid_pixel,(yy+1)*grid_pixel), font, 1,(255,255,255),2,cv2.LINE_AA)
              arr[yy][xx] = index
              out_str +=str(index)
              cv2.imshow('number',img_source) 
      

       cv2.imwrite('result.png',img_source)
       print arr

       #print out_str
       point_str  =bead_algorithm.calc_path(out_str,point_num)
       print point_str
       # move to first bead
       str_first_bead = 'G0 X27 Y-55\n'  
       ser.write(str_first_bead)
       while(((ser.readline()).strip()) != 'ok'):
          time.sleep(1) 
       print ':' + 'ok'
       old_x = 0
       old_y = 0

       for i in range (0,point_num-1,1):
          line_sx = int(point_str[2*i]) * grid_pixel + grid_pixel/2  
          line_sy = int(point_str[2*i+1]) * grid_pixel + grid_pixel/2 
          line_ex = int(point_str[2*i+2]) * grid_pixel + grid_pixel/2 
          line_ey = int(point_str[2*i+3]) * grid_pixel + grid_pixel/2 
          cv2.line(img_source2,(line_sx,line_sy),(line_ex,line_ey),(0,0,255),5) 

       cv2.imshow('path',img_source2) 
       for i in range (0,point_num):
           y_coord = (int(point_str[2*i]) - old_y) * bead_screen 
           x_coord = (int(point_str[2*i+1]) - old_x ) * bead_screen
           old_y = int(point_str[2*i])
           old_x = int(point_str[2*i+1]) 
           str_ = 'G0 X' + str(x_coord) + ' Y' + str(y_coord)  + '\n'  
           print str_
           ser.write(str_)
           while(((ser.readline()).strip()) != 'ok'):
               time.sleep(1) 
           print ':' + 'ok'

           if(i==0):
               # down the pen
               ser.write('G0 Z' + str(z_num) +'\n')
               while(((ser.readline()).strip()) != 'ok'):
                  time.sleep(1)
               print ':' + 'ok'
     
       # up the pen
       ser.write('G0 Z-' + str(z_num) + '\n')
       while(   ((ser.readline()).strip()) != 'ok'):
          time.sleep(1)
       print ':' + 'ok'

       # move to first bead
       y_coord = (0 - old_y) * bead_screen 
       x_coord = (0 - old_x ) * bead_screen
       str_ = 'G0 X' + str(x_coord) + ' Y' + str(y_coord)  + '\n'
       print str_
       ser.write(str_)
       while(   ((ser.readline()).strip()) != 'ok'):
           time.sleep(1) 
       print ':' + 'ok'


       #move back to home
       str_first_bead = 'G0 X-27 Y55\n'  
       ser.write(str_first_bead)
       while(   ((ser.readline()).strip()) != 'ok'):
           time.sleep(1) 
       print ':' + 'ok'

ser.close()
# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()

f = open('pos.txt', 'w')

f.write(str(pos0_x) + '\n')
f.write(str(pos0_y) + '\n')
f.write(str(pos1_x) + '\n')
f.write(str(pos1_y) + '\n')
f.write(str(pos2_x) + '\n')
f.write(str(pos2_y) + '\n')
f.write(str(pos3_x) + '\n')
f.write(str(pos3_y) + '\n')
f.close();


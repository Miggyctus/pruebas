import serial
import argparse
import cv2
import time
import numpy as np
import math
import mediapipe as mp
import random
import pygame

########## 手部追蹤偵測 #############
class handDetector():
    def __init__(self, mode=False, maxHands=2, detectionCon=0.5, trackCon=0.5):
        self.mode = mode
        self.maxHands = maxHands
        self.detectionCon = detectionCon
        self.trackCon = trackCon

        self.mpHands = mp.solutions.hands
        # self.hands = self.mpHands.Hands(self.mode, self.maxHands,
        #                                 self.detectionCon, self.trackCon)
        
        self.hands = self.mpHands.Hands(self.mode, self.maxHands,
                                    min_detection_confidence=self.detectionCon,
                                    min_tracking_confidence=self.trackCon)

        self.mpDraw = mp.solutions.drawing_utils
    def findHands(self, img, draw=True):
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        self.results = self.hands.process(imgRGB)
        # print(results.multi_hand_landmarks)

        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                    self.mpDraw.draw_landmarks(img, handLms,
                                               self.mpHands.HAND_CONNECTIONS)
        return img

    def findPosition(self, img, handNo=0, draw=True):

        lmList = []
        if self.results.multi_hand_landmarks:
            myHand = self.results.multi_hand_landmarks[handNo]
            for id, lm in enumerate(myHand.landmark):
                # print(id, lm)
                h, w, c = img.shape
                cx, cy = int(lm.x * w), int(lm.y * h)
                # print(id, cx, cy)
                lmList.append( [id, cx, cy])
                if draw:
                    cv2.circle(img, (cx, cy), 15, (255, 0, 255), cv2.FILLED)
        return lmList
    
def play_background_music():
    pygame.mixer.music.load("bgm.wav")
    pygame.mixer.music.play(-1)

def play_good_sound():
    good_sound = pygame.mixer.Sound("good.wav")
    good_sound.play()

def play_bad_sound():
    bad_sound = pygame.mixer.Sound("bad.wav")
    bad_sound.play()  
                        
def main():

    pygame.init()

    # 設定音量（0.0 到 1.0 之間）
    pygame.mixer.music.set_volume(0.8)
    
############## 各參數設定 ##################
    pTime  = 0
    minPwm = 0
    maxPwm = 255
    briArd = 0
    briBar = 400
    briPer = 0
    
############## 指定WEBCAM和Arduino Serial Port編號的指令 ##################
    
    parser = argparse.ArgumentParser(
      formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
    '--video', help='Video number', required=False, type=int, default=0)
    parser.add_argument(
      '--com', help='Number of UART prot.', required=False)
    args = parser.parse_args()
    
    COM_PORT = 'COM'+str(args.com)
    BAUD_RATES = 9600
    ser = serial.Serial(COM_PORT, BAUD_RATES)
    
    args = parser.parse_args()
    
    
############## WEBCAM相關參數定義 ##################
    height = 640; width=1080
    wCam, hCam = height, width   
    cap = cv2.VideoCapture(args.video) # 攝影機編號預設為0，也可以輸入其他編號!
    cap.set(3, wCam)
    cap.set(4, hCam)
    detector = handDetector(detectionCon=0.7)

    count=0
    score=0
    hp=3

    circle_radius = 50
    circle_y, circle_x=50,random.randint(circle_radius, (width - circle_radius)*0.6)
    circle_color = (random.randint(0, 255), random.randint(0, 255), random.randint(0,255))
    finger_radius = 20
    finger_touching=False    
    alpha = 0.99 # 設定透明度，這裡設定為50%
    play_background_music()

    try:
        
        while True:
            success, img = cap.read()

            img = cv2.flip(img, 1)  # 加入這行進行左右翻轉

            img = detector.findHands(img)
            mask = np.zeros(img.shape, dtype=np.uint8)
            lmList = detector.findPosition(img, draw=False)
            #print(lmList)
            light=0        
            
            # 畫圓點           
            circle_center = (circle_x,circle_y)
            cv2.circle(mask, circle_center, circle_radius, circle_color, -1)
            cv2.putText(mask, f'{count}', circle_center, cv2.FONT_HERSHEY_COMPLEX, 3, (255,255,255),3)
            #圓點自由落體            
            circle_y+=3 #7

            #掉到最下面            
            if circle_y>=(height - circle_radius)*0.7:
                circle_y, circle_x=50,random.randint(circle_radius, (width - circle_radius)*0.6)
                circle_color = (random.randint(100, 200), random.randint(100, 200), random.randint(100,200))
                count=0
                alpha=0.99
                hp-=1
                if hp>0:
                    play_bad_sound()
            
            # 顯示文字
            #cv2.putText(img,f'circle_position{circle_center}',(40,150), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 0), 3)
            #cv2.putText(img, f'alpha {alpha}', (40, 200), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 0), 3)
            #cv2.putText(img, f'count {count}', (40, 100), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 0),3)
            cv2.putText(img, f'Score: {score}', (40, 100), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 153, 255),3)
            cv2.putText(img, f'HP: {hp}', (40, 150), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 255),3)
            #cv2.putText(img, f'color: {circle_color}', (40, 200), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 255),3)
            
            if len(lmList) != 0:
                x,y=lmList[8][1],lmList[8][2]
                finger_position=(x,y)
                cv2.circle(img,finger_position,finger_radius,255,-1)
                #cv2.putText(img,f'finger_position{finger_position}',(40,300), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 0), 3)

                # 計算食指位置和圓點位置的距離
                distance = math.sqrt((finger_position[0] - circle_center[0])**2 + (finger_position[1] - circle_center[1])**2)
                #cv2.putText(img, f'distance {distance}', (40, 250), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 0),3)

                #判斷是否碰觸
                if distance < circle_radius + finger_radius and not finger_touching:
                    alpha -= 0.33
                    count += 1
                    
                    
                    finger_touching = True
                    
                elif distance >= (circle_radius + finger_radius)*2 and finger_touching:
                    finger_touching = False

            # 碰三次   
            if count==3:
                circle_color = (random.randint(0, 255), random.randint(0, 255), random.randint(0,255))           
                #circle_y, circle_x=random.randint(circle_radius, (height - circle_radius)*0.7),random.randint(circle_radius, (width - circle_radius)*0.6)
                circle_y, circle_x=50,random.randint(circle_radius, (width - circle_radius)*0.6)
                circle_center = (circle_x,circle_y)
                
                #time.sleep(0.5)
                count=0
                alpha=0.99
                score+=1
                light=1
                play_good_sound()              
    
            #送出數值給Arduino
            ser.write(str(light).encode())
      
            #計算每秒跑幾張
            cTime = time.time()
            fps = 1 / (cTime - pTime)
            pTime = cTime
            cv2.putText(img, f'FPS: {int(fps)}', (40, 50), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 0, 0), 3)
        
            if hp<=0 :
                result = np.zeros(img.shape, dtype=np.uint8)
                cv2.putText(result, f'GAME OVER', (150, int(height*0.6/2)), cv2.FONT_HERSHEY_COMPLEX, 2, (0, 0, 255), 1)
                cv2.putText(result, f'Score: {score}', (50, int(height*0.4-200)), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 153, 255), 3)  
                pygame.mixer.music.stop()  # 程式結束時停止背景音樂       
            else:
                # 使用 addWeighted 函數混合圖像
                result = cv2.addWeighted(img, 1, mask, alpha, 0)

            
            #顯示畫面 
            cv2.imshow("HandDetector", result)
                
            #按q停止程式          
            if cv2.waitKey(10) & 0xFF == ord('q'):
                break
    except KeyboardInterrupt:
        ser.close()
        cap.release()
        cv2.destroyAllWindows()
    
if __name__ == '__main__' :
    main()
# 1) 현재 강아지 감지시 스텝모터 및 서보모터는 동작하지만
# 양방향이 아닌 한방향으로 동작
# 2) 프로그램 종료시 서보및 스텝모터 원위치 복귀동작 미흡

import Jetson.GPIO as GPIO
import time
import cv2
import numpy as np
from ultralytics import YOLO
import sys


# GPIO 경고 끄기
GPIO.setwarnings(False)

# GPIO 핀 설정
GPIO.setmode(GPIO.BOARD)

# X축 스텝모터 핀 (핀 4개)
IN1 = 11
IN2 = 12
IN3 = 15
IN4 = 16


# Y축 서보모터 핀
SERVO_PIN = 32  # 서보모터 제어 핀

# 스텝모터 핀 출력 설정

GPIO.setup(IN1, GPIO.OUT)
GPIO.setup(IN2, GPIO.OUT)
GPIO.setup(IN3, GPIO.OUT)
GPIO.setup(IN4, GPIO.OUT)

# 서보모터 핀 출력 설정
GPIO.setup(SERVO_PIN, GPIO.OUT)

# 서보모터 PWM 설정
servo = GPIO.PWM(SERVO_PIN, 50)  # 서보모터 50Hz PWM
servo.start(0)  # 서보모터 시작, 초기 듀티사이클 0

# 스텝모터 제어 시퀀스 (한 바퀴 회전)
step_sequence = [
    [1, 0, 0, 1],
    [1, 0, 0, 0],
    [1, 1, 0, 0],
    [0, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 1, 0],
    [0, 0, 1, 1],
    [0, 0, 0, 1]
]

# 스텝모터 제어 함수
def step_motor(steps, delay=0.01):
    for _ in range(steps):
        for step in step_sequence:
            GPIO.output(IN1, step[0])
            GPIO.output(IN2, step[1])
            GPIO.output(IN3, step[2])
            GPIO.output(IN4, step[3])
           
            time.sleep(delay)  # 속도 조절

# 서보모터 제어 함수
def move_servo(angle):
    # 서보모터 각도를 0도에서 180도 사이로 제한
    if angle < 0:
        angle = 0
    elif angle > 180:
        angle = 180
    duty = angle / 18 + 2

    servo.ChangeDutyCycle(duty)
    time.sleep(0.5)
    servo.ChangeDutyCycle(0)
    
# 스텝모터와 서보모터를 원래 위치로 되돌리는 함수
def reset_motors():
    print("모터를 초기 위치로 복귀 중...")
    move_servo(90)  # 서보모터를 90도로 이동 (중앙 위치)
    
    # 스텝모터를 초기 위치로 복귀 (정해진 스텝 수만큼 역방향 이동)
    step_motor(steps=100, delay=0.01)  # 100 스텝 후진 (필요에 따라 값 조절)
    print("모터 복귀 완료.")
    

# 강아지 탐지 및 모터 제어를 위한 YOLO 모델 로드
model = YOLO('/home/jetson/project/yolov8n.pt')

# 추적할 강아지 클래스 ID 및 신뢰도 임계치 설정
DOG_CLASS_ID = 16
CONFIDENCE_THRESHOLD = 0.1

# 메인 추론 및 모터 제어 함수
def run_inference():

    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("웹캠을 켜는데 오류가 발생했습니다.")
            return

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                print("웹캠이 종료되었습니다.")
                continue
            
            results = model(frame)

            # 감지된 객체의 바운딩 박스와 신뢰도 추출
            boxes = results[0].boxes.xyxy.cpu().numpy()  # 바운딩 박스 좌표
            confidences = results[0].boxes.conf.cpu().numpy()  # 신뢰도
            class_ids = results[0].boxes.cls.cpu().numpy()  # 클래스 ID

            # 추적할 강아지 객체 선택
            highest_confidence = 0
            best_box = None

            for i, bbox in enumerate(boxes):
                class_id = int(class_ids[i])  # 클래스 ID 추출
                confidence = confidences[i]  # 신뢰도 추출

                if class_id == DOG_CLASS_ID and confidence > CONFIDENCE_THRESHOLD:
                    if confidence > highest_confidence:
                        highest_confidence = confidence
                        best_box = bbox  # 가장 신뢰도가 높은 강아지 추적

            if best_box is not None:
                # 강아지 바운딩 박스 및 신뢰도 출력
                x_min, y_min, x_max, y_max = best_box[:4]
                cv2.rectangle(frame, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)
                cv2.putText(frame, f'Dog: {highest_confidence:.2f}', (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

                # 바운딩 박스 중심점 계산
                center_point = ((x_min + x_max) / 2, (y_min + y_max) / 2)

                # 목표 중심점 설정 (화면의 중앙)
                target_x = frame.shape[1] // 2
                target_y = frame.shape[0] // 2

                # 현재 중심점과 목표 중심점 사이의 차이 계산
                error_x = target_x - center_point[0]
                error_y = target_y - center_point[1]

                 # 스텝모터 제어 (X축)
                if abs(error_x) > 10:  # 10 픽셀 이상의 차이가 나면 움직임
                    step_motor(steps=10)  # 스텝모터 회전

                # 서보모터 제어 (Y축)
                if abs(error_y) > 10:
                    angle = 90 + (error_y / 10)  # 서보모터 각도 계산 (90도를 중심으로 이동)
                    move_servo(angle)

            
            # 프레임 결과 출력
            cv2.imshow('frame', frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
            
    except Exception as e:
        print(f"Error during inference: {e}")

    finally:
        cap.release()
        cv2.destroyAllWindows()


   

    # # # FPS 계산 및 출력
    # stop_time = time.time()
    # total_time = stop_time - start_time
    # fps = frame_number / total_time
    # print(f"FPS: {fps:.2f}")
    # return fps


# 프로그램 종료 시 GPIO 정리
if __name__ == "__main__":
    try:
        run_inference()
    finally:
        try:
            reset_motors()  # 프로그램 종료 시 모터를 원래 자리로 되돌림
            if 'servo' in globals():
                servo.stop()  # 서보모터 PWM 중지
            GPIO.cleanup()  # GPIO 핀 정리
        except Exception as e:
            print(f"Error cleaning up GPIO: {e}")
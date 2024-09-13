# -*- coding: utf-8 -*-
import Jetson.GPIO as GPIO
import time
from signal import signal, SIGINT
import sys

# 핀 번호 설정
SERVO_PIN = 32  # 서보모터의 핀 번호
STEP_PIN1 = 11  # 스텝모터의 핀 번호 1
STEP_PIN2 = 12  # 스텝모터의 핀 번호 2
STEP_PIN3 = 15  # 스텝모터의 핀 번호 3
STEP_PIN4 = 16  # 스텝모터의 핀 번호 4

# GPIO 모드 설정
GPIO.setmode(GPIO.BOARD)
GPIO.setup(SERVO_PIN, GPIO.OUT)
GPIO.setup(STEP_PIN1, GPIO.OUT)
GPIO.setup(STEP_PIN2, GPIO.OUT)
GPIO.setup(STEP_PIN3, GPIO.OUT)
GPIO.setup(STEP_PIN4, GPIO.OUT)

# PWM 설정
servo_pwm = GPIO.PWM(SERVO_PIN, 50)  # 50Hz 주파수
servo_pwm.start(0)  # 초기 서보 모터 위치 설정

# 스텝모터의 단계 설정
step_sequence = [
    [1, 0, 0, 0],
    [1, 1, 0, 0],
    [0, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 1, 0],
    [0, 0, 1, 1],
    [0, 0, 0, 1],
    [1, 0, 0, 1]
]

# 클래스 정의
class MotorController:
    def __init__(self):
        self.hor_degree = 90  # 초기 서보모터 각도
        
    # 서보모터의 각도 설정 함수
    def set_servo_degree(self, degree):
        duty = degree / 18 + 2
        duty = max(2, min(12, duty))
        servo_pwm.ChangeDutyCycle(duty)
        time.sleep(1)
        servo_pwm.ChangeDutyCycle(0)

#스텝모터를 회전시키는 함수
    def step(self, direction):
        steps = 512  # 예시로 512 스텝 이동
        delay = 0.01
        if direction == 0:
            # 시계방향
            for _ in range(steps):
                for step in step_sequence:
                    GPIO.output(STEP_PIN1, step[0])
                    GPIO.output(STEP_PIN2, step[1])
                    GPIO.output(STEP_PIN3, step[2])
                    GPIO.output(STEP_PIN4, step[3])
                    time.sleep(delay)
        elif direction == 1:
            # 반시계방향
            for _ in range(steps):
                for step in reversed(step_sequence):
                    GPIO.output(STEP_PIN1, step[0])
                    GPIO.output(STEP_PIN2, step[1])
                    GPIO.output(STEP_PIN3, step[2])
                    GPIO.output(STEP_PIN4, step[3])
                    time.sleep(delay)

    def degree_clamp(self, degree):
        return max(0, min(180, degree))

    def input(self, up, down, left, right):
        if up:
            self.hor_degree += 2
        if down:
            self.hor_degree -= 2
        if left:
            self.step(0)
        if right:
            self.step(1)
        self.hor_degree = self.degree_clamp(self.hor_degree)
        self.set_servo_degree(self.hor_degree)

    def decode(self, key):
        up = down = left = right = 0
        if key == 'q':
            up = 1
            left = 1
        elif key == 'w':
            up = 1
        elif key == 'e':
            up = 1
            right = 1
        elif key == 'a':
            left = 1
        elif key == 'd':
            right = 1
        elif key == 'z':
            left = 1
            down = 1
        elif key == 'x':
            down = 1
        elif key == 'c':
            down = 1
            right = 1
        
        # Update motors based on decoded input
        self.input(up, down, left, right)

def cleanup():
    print("프로그램 종료. 모터를 원래 위치로 이동합니다.")
    # 서보모터를 원래 위치로 이동
    controller.set_servo_degree(90)
    # 스텝모터 멈추기
    GPIO.output(STEP_PIN1, 0)
    GPIO.output(STEP_PIN2, 0)
    GPIO.output(STEP_PIN3, 0)
    GPIO.output(STEP_PIN4, 0)
    # PWM 종료
    servo_pwm.stop()
    # GPIO 핀 정리
    GPIO.cleanup()
    sys.exit(0)  # 강제 종료

# 종료 시 호출될 함수 등록
def signal_handler(sig, frame):
    cleanup()

signal(SIGINT, signal_handler)

# 사용자 입력을 통한 제어
def main():
    global controller
    controller = MotorController()
    
    print("서보모터와 스텝모터 제어 프로그램입니다.")
    print("서보모터 각도 조정 및 스텝모터 제어를 위한 키를 입력하세요.")
    print("q: up + left, w: up, e: up + right, a: left, d: right, z: down + left, x: down, c: down + right")
    print("프로그램을 종료하려면 Ctrl+C를 누르세요.")
    
    while True:
        try:
            key = input("명령어를 입력하세요: ").strip().lower()
            controller.decode(key)
        except KeyboardInterrupt:
            cleanup()
        except Exception as e:
            print("예상치 못한 오류가 발생했습니다: {e}")

if __name__ == "__main__":
    main()


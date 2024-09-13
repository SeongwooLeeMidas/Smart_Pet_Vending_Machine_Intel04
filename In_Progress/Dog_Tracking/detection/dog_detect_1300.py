import cv2
import sys
import torch
import numpy as np
import time
from ultralytics import YOLO

# YOLOv8 모델 로드 (best.pt 경로를 YOLOv8으로 학습한 모델 경로로 설정)
model = YOLO('/home/jetson/dog_detect/best.pt')

# 강아지 바운딩 박스 색상 정의
color = (0, 255, 0)  # 초록색으로 바운딩 박스

# 메인 추론 함수 정의
def run_inference():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("웹캠을 켜는데 오류가 발생했습니다.")
        return

    # FPS 계산을 위한 변수 초기화
    start_time = time.time()
    frame_number = 0

    # 추적기 초기화 (하나의 객체만 추적)
    tracker = None
    tracking = False

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                break

            if not tracking:
                # YOLOv8을 사용한 첫 추론
                results = model(frame)

                # 결과 처리: results[0].boxes.xyxy를 사용해 바운딩 박스 좌표 추출
                detections = results[0].boxes.xyxy.cpu().numpy() if len(results) > 0 else []

                if len(detections) > 0:
                    # 첫 번째 검출된 객체만 추적 (강아지 하나만 추적)
                    x_min, y_min, x_max, y_max = detections[0][:4]
                    confidence = results[0].boxes.conf[0].item()

                    if confidence > 0.5:  # 신뢰도 임계값 설정
                        tracker = cv2.TrackerCSRT_create()
                        tracker.init(frame, (x_min, y_min, x_max - x_min, y_max - y_min))  # 추적기 초기화
                        tracking = True

            if tracking:
                # 추적 중인 객체가 있다면 추적 업데이트
                ret, bbox = tracker.update(frame)
                if ret:
                    # 추적된 객체 사각형 표시
                    p1 = (int(bbox[0]), int(bbox[1]))
                    p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
                    cv2.rectangle(frame, p1, p2, color, 2, 1)
                else:
                    # 추적이 실패한 경우 다시 객체 검출 시도
                    tracking = False

            # 프레임 수 계산
            frame_number += 1

            # cv2.imshow() 대신 주석 처리하여 창을 열지 않음
            cv2.imshow('YOLOv8 Dog Detection & Tracking', frame)

            key = cv2.waitKey(1)
            if key == 27:  # ESC 키를 눌러 종료
                break

    except KeyboardInterrupt:
        print("중단됨")
    except RuntimeError as e:
        print(e)
    except Exception as e:
        print(f"예기치 못한 오류 발생: {e}")

    cap.release()
    cv2.destroyAllWindows()

    # FPS 계산 및 출력
    stop_time = time.time()
    total_time = stop_time - start_time
    fps = frame_number / total_time
    print(f"FPS: {fps:.2f}")
    return fps

if __name__ == "__main__":
    fps = run_inference()
    sys.exit(0)

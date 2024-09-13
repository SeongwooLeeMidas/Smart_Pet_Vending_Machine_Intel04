# dog_detect_1140

import cv2
import sys
import torch
import numpy as np
import time 
from ultralytics import YOLO

# YOLOv8 모델 로드 (best.pt 경로를 YOLOv8으로 학습한 모델 경로로 설정)
model = YOLO('/home/jetson/dog_detect/best.pt')

# 고정된 입력 크기
input_height = 640
input_width = 640

# 각 객체에 대한 색상 정의
tracker_colors = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0)]

# 메인 추론 함수 정의
def run_inference():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("웹캠을 켜는데 오류가 발생했습니다.")
        return
        
    # FPS 계산을 위한 변수 초기화
    start_time = time.time()
    frame_number = 0

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            
            # Yolov8 추론
            results = model(frame)
            
            # 결과를 numpy 배열로 변환
            detections = np.array(results.xyxy[0].cpu())  # x_min, y_min, x_max, y_max, confidence, class

            for detection in detections:
                x_min, y_min, x_max, y_max, _ = detection
                tracker.init(frame, (x_min, y_min, x_max - x_min, y_max - y_min))  # 추적 초기화

                # 추적 업데이트
                ret, bbox = tracker.update(frame)
                if ret:
                    
                    # 추적된 객체 사각형 표시
                    p1 = (int(bbox[0]), int(bbox[1]))
                    p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
                    cv2.rectangle(frame, p1, p2, tracker_colors[0], 2, 1)
            
            for i, (x_min, y_min, x_max, y_max, confidence) in enumerate(detections):
                color = tracker_colors[i % len(tracker_colors)]
                cv2.rectangle(frame, (x_min, y_min), (x_max, y_max), color, 2)
                cv2.putText(frame, f"dog {i}: {confidence:.2f}", (x_min, y_min - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

            # 프레임 수 계산
            frame_number += 1
        
            cv2.imshow('frame', frame)

            key = cv2.waitKey(1)
            if key == 27:
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
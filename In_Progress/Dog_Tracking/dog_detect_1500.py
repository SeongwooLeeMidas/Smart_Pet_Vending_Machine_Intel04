#dog_detect_1500

import cv2
import sys
import numpy as np
import time 
from ultralytics import YOLO

# YOLOv8 모델 로드 (best.pt 경로를 YOLOv8으로 학습한 모델 경로로 설정)
model = YOLO('/home/jetson/dog_detect/best.pt')


# 추적기 초기화
trackers = []
tracker_colors = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0)]
drone_id = 1

# 메인 추론 함수 정의
def run_inference():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("웹캠을 켜는데 오류가 발생했습니다.")
        return
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # YOLOv8을 사용한 객체 탐지
        resized_frame = cv2.resize(frame, (640, 640))    
        
        # YOLOv8을 사용한 객체 탐지
        results = model(resized_frame)

        detections = results[0].boxes.xyxy.cpu().numpy()  # YOLOv8에서 바운딩 박스 정보 추출
        confidences = results[0].boxes.conf.cpu().numpy()  # YOLOv8에서 신뢰도 정보 추출

        print(f"Detections: {detections}, Confidences: {confidences}")

        # 신뢰도가 0.5 이상인 경우만 바운딩 박스를 그림
        for i, bbox in enumerate(detections):
            x_min, y_min, x_max, y_max = bbox[:4]  # 바운딩 박스 좌표 추출
            confidence = confidences[i]  # 신뢰도 추출

            if confidence > 0.3:  # 신뢰도 임계값 0.5 이상만 처리
                cv2.rectangle(frame, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 255), 2)
                cv2.putText(frame, f'Dog: {confidence:.2f}', (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 255), 2)

        # 프레임 표시
        cv2.imshow('Dog Tracking', frame)

        if cv2.waitKey(1) == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    run_inference()
    sys.exit(0)
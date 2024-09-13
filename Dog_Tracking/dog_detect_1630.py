#dog_detect_1630

import cv2
import sys
import numpy as np
import time 
from ultralytics import YOLO

# YOLOv8 모델 로드 (best.pt 경로를 YOLOv8으로 학습한 모델 경로로 설정)
model = YOLO('/home/jetson/dog_detect/yolov8n.pt')

# 추적기 초기화
trackers = []
tracker_colors = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0)]  # 각 드론에 대한 색상 정의
dog_id = 1

# 메인 추론 함수 정의
def run_inference():
    global dog_id

    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("웹캠을 켜는데 오류가 발생했습니다.")
            return

        ret, frame = cap.read()
        if not ret:
            print("웹캠을 켜는데 실패했습니다.")
            exit(1)

        cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
        start_time = time.time()
        frame_number = 0

        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                print("웹캠이 종료되었습니다.")
                continue
            
            results = model(frame)

            # 감지된 객체의 바운딩 박스와 신뢰도 추출
            boxes = results[0].boxes.xyxy.cpu().numpy()  # 바운딩 박스 좌표
            confidences = results[0].boxes.conf.cpu().numpy()  # 신뢰도

            # 신뢰도가 0.5 이상인 경우만 바운딩 박스를 그림
            for i, bbox in enumerate(boxes):
                x_min, y_min, x_max, y_max = bbox[:4]  # 바운딩 박스 좌표 추출
                confidence = confidences[i]  # 신뢰도 추출
            
                if confidence > 0.5:
                    cv2.rectangle(frame, (int(x_min), int(y_min)), (int(x_max), int(y_max)), (0, 255, 0), 2)
                    cv2.putText(frame, f'Dog {i+1}: {confidence:.2f}', (int(x_min), int(y_min) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

            # 프레임 수 계산
            frame_number +=1
        
            cv2.imshow('frame', frame)

            key = cv2.waitKey(1)
            if key == 27:
                break

    except KeyboardInterrupt:
        print("중단됨")
    except RuntimeError as e:
        print (e)

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
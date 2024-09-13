import datetime
import cv2
from ultralytics import YOLO
from deep_sort_realtime.deepsort_tracker import DeepSort

CONFIDENCE_THRESHOLD = 0.3  # 신뢰도 임계값을 낮춰서 테스트
GREEN = (0, 255, 0)
WHITE = (255, 255, 255)

class_list = ['dog']

model = YOLO('/home/jetson/dog_detect/yolov8n.pt')
tracker = DeepSort(max_age=50)

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

while True:
    start = datetime.datetime.now()

    ret, frame = cap.read()
    if not ret:
        print('웹캠을 켜는데 오류가 발생했습니다.')
        break

    # YOLO 모델 예측
    detection = model.predict(source=[frame], save=False)[0]
    results = []
    
    # 모델 예측 결과 출력
    print("Detection Results:", detection)

    if len(detection.boxes) == 0:
        print("No detections found!")
        continue  # 감지된 객체가 없으면 넘어감

    # 예측된 객체의 바운딩 박스 처리
    for data in detection.boxes.data.tolist():  # data : [xmin, ymin, xmax, ymax, confidence_score, class_id]
        confidence = float(data[4])
        if confidence < CONFIDENCE_THRESHOLD:
            continue

        xmin, ymin, xmax, ymax = int(data[0]), int(data[1]), int(data[2]), int(data[3])
        label = int(data[5])
        
        # 바운딩 박스 및 텍스트 출력
        print(f"Detected: {class_list[label]}, Confidence: {confidence}")
        cv2.rectangle(frame, (xmin, ymin), (xmax, ymax), (0, 255, 0), 2)
        cv2.putText(frame, class_list[label]+' '+str(round(confidence, 3)) + '%', (xmin, ymin), cv2.FONT_ITALIC, 1, (255, 255, 255), 2)

        results.append([[xmin, ymin, xmax-xmin, ymax-ymin], confidence, label])

    # 추적기 업데이트
    tracks = tracker.update_tracks(results, frame=frame)

    for track in tracks:
        if not track.is_confirmed():
            continue

        track_id = track.track_id
        ltrb = track.to_ltrb()

        xmin, ymin, xmax, ymax = int(ltrb[0]), int(ltrb[1]), int(ltrb[2]), int(ltrb[3])
        cv2.rectangle(frame, (xmin, ymin), (xmax, ymax), GREEN, 2)
        cv2.rectangle(frame, (xmin, ymin - 20), (xmin + 20, ymin), GREEN, -1)
        cv2.putText(frame, str(track_id), (xmin + 5, ymin - 8), cv2.FONT_HERSHEY_SIMPLEX, 0.5, WHITE, 2)

    end = datetime.datetime.now()

    total = (end - start).total_seconds()
    fps = f'FPS: {1 / total:.2f}'
    cv2.putText(frame, fps, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

    cv2.imshow('frame', frame)

    if cv2.waitKey(1) == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

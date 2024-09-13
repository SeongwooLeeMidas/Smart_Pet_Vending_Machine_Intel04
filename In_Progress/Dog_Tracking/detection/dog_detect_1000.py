import cv2
import sys
import time 

# 모델 경로 정의
model_xml = "/home/ubuntu/workspace1/otx_detection/outputs/D-gaja/Chanuks/0626_yolox/deploy/model/model.xml"
model_bin = "/home/ubuntu/workspace1/otx_detection/outputs/D-gaja/Chanuks/0626_yolox/deploy/model/model.bin"

# OpenVINO core 초기화
ie = Core()

# 모델 로드 및 네트워크 컴파일
try: 
    model = ie.read_model(model=model_xml, weights=model_bin)
    compiled_model = ie.compile_model(model=model, device_name="CPU")
except Exception as e:
    print(f"모델 로드 오류: {e}")
    sys.exit(1)
    
input_layer = compiled_model.input(0)
output_layer = compiled_model.output(0)

# 고정된 입력 크기
input_height = 640
input_width = 640

# 각 드론에 대한 색상 정의
tracker_colors = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0)]

# 입력 프레임 전처리 함수 정의
def preprocess_frame(frame):
    resized_frame = cv2.resize(frame, (input_width, input_height))
    transposed_frame = np.transpose(resized_frame, (2, 0, 1))  # (H, W, C) to (C, H, W)
    input_data = np.expand_dims(transposed_frame, axis=0)  # (C, H, W) to (1, C, H, W)
    input_data = input_data.astype(np.float32)  # 모델이 float32 타입의 입력을 기대할 수 있으므로 변환
    return input_data

# 모델 출력 후처리 함수 정의
def postprocess_output(frame, boxes, conf_threshold=0.5):
    frame_height, frame_width = frame.shape[:2]
    detections = []
    for box in boxes:
        x_min, y_min, x_max, y_max, confidence = box
        if confidence > conf_threshold:
            x_min = int(x_min * frame_width / input_width)
            y_min = int(y_min * frame_height / input_height)
            x_max = int(x_max * frame_width / input_width)
            y_max = int(y_max * frame_height / input_height)
            detections.append((x_min, y_min, x_max, y_max, confidence))
    return detections

# 메인 추론 함수 정의
def run_inference():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("웹캠을 켜는데 오류가 발생했습니다.")
        return
        
    # FPS 계산을 위한 변수 초기화
    start_time = time.time()
    frame_number = 0

    # 추적기 초기화
    tracker = cv2.TrackerCSRT.create()

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            
            # 드론 검출
            input_data = preprocess_frame(frame)
            results = compiled_model([input_data])
            boxes = results[output_layer][0]
            
            detections = postprocess_output(frame, boxes)

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
                cv2.putText(frame, f"Drone {i}: {confidence:.2f}", (x_min, y_min - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

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
import socket
import cv2
import numpy as np
import tensorflow as tf
import time
import warnings
import struct
import os
warnings.filterwarnings("ignore", category=UserWarning)

# 멀티스레딩 설정 (CPU 코어 수에 맞게 조정)
tf.config.threading.set_intra_op_parallelism_threads(4)
tf.config.threading.set_inter_op_parallelism_threads(4)

# TensorFlow .h5 모델 로드
print("Loading TensorFlow .h5 model...")
model = tf.keras.models.load_model('EfficientNetB0-dog-breeds-97.54.h5')

print("Model loaded.")

# 클래스 번호와 품종 이름을 매칭하는 딕셔너리
class_names = {
    #0: 'BIC',  # Bichon Frise
    0: 'CHL',  # Chihuahua
    1: 'DRI',  # Dachshund
    2: 'LAB',  # Labrador Retriever
    #4: 'MAL',  # Maltese
    3: 'POM',  # Pomeranian
    #6: 'POO',  # Poodle
    #7: 'TER'   # Terrier
}

# 20번의 예측 확률을 저장할 리스트 (각 클래스에 대해 20개의 확률 값을 저장할 배열)
prediction_history = np.zeros((5, 4))

prediction_count = 0
count_threshold = 5  # 예측 횟수 임계값

# 품종이 확정되면 클라이언트에게 'STOP' 명령 전송
def send_stop_detection_command():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect(('localhost', 5000))  # 클라이언트가 RECO 명령을 수신하는 포트로 접속
        client_socket.sendall(b'STOP')  # 'STOP' 명령 전송
        print("Sent STOP command to client.")
    except Exception as e:
        print(f"Failed to send STOP command: {e}")
    finally:
        client_socket.close()

# 확정된 품종을 7000번 포트로 C 클라이언트에 전송
def send_confirmed_breed(breed):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect(('localhost', 7000))  # C 클라이언트가 7000번 포트에서 대기 중
        message = f"[KJD_QT]BREED@{breed}"
        client_socket.sendall(message.encode('utf-8'))  # 품종 메시지 전송
        print(f"Sent breed message: {message}")
    except Exception as e:
        print(f"Failed to send breed message: {e}")
    finally:
        client_socket.close()

file_path = '../images/cropped_frame.jpg'

while True:
    try:
        # 파일이 존재하는지 확인
        if os.path.exists(file_path):
            frame = cv2.imread(file_path)
            if frame is None:
                print("Failed to load image")
                time.sleep(1)
                continue
        else:
            print("waiting image..")
            time.sleep(1)
            continue

        # 이미지를 화면에 표시
        cv2.imshow('Input Image', frame)
        cv2.waitKey(500)

        # 이미지 전처리 (224x224 크기로 조정)
        img_array = cv2.resize(frame, (224, 224))
        img_array = img_array.astype(np.float32) / 255.0
        img_array = np.expand_dims(img_array, axis=0)  # 배치 차원 추가

        print("Image pre-processing completed. Starting inference...")

        # 추론 시간 측정 시작
        start_time = time.time()

        # 모델 추론 수행
        predictions = model.predict(img_array)

        # 예측 결과 저장
        prediction_history[prediction_count % count_threshold] = predictions[0]  # 최근 예측 확률 저장
        prediction_count += 1

        # 예측 결과 출력
        for i, probability in enumerate(predictions[0]):
            predicted_breed = class_names.get(i, "Unknown")
            print(f'Class {i} ({predicted_breed}): {probability:.4f}')

        # 추론 시간 측정 종료
        end_time = time.time()
        elapsed_time = end_time - start_time
        print(f'Inference completed. Time taken for prediction: {elapsed_time:.4f} seconds')

        # 20번 예측이 완료되었을 때 편차 계산
        if prediction_count >= count_threshold:
            # 각 클래스의 예측 확률 편차 계산
            std_devs = np.std(prediction_history, axis=0)
            # 편차가 작은 순서대로 인덱스를 정렬
            sorted_indices = np.argsort(std_devs)

            # 첫 번째로 작은 편차의 인덱스 (즉, 가장 작은 편차)
            final_breed_index = sorted_indices[0]

            # 두 번째로 작은 편차의 인덱스
            second_breed_index = sorted_indices[1]

            # 두 품종 이름 출력
            final_breed = class_names.get(final_breed_index, "Unknown")
            second_breed = class_names.get(second_breed_index, "Unknown")

            print(f"Final breed confirmed: {final_breed} (smallest standard deviation)")
            print(f"Second breed confirmed: {second_breed} (second smallest standard deviation)")
            # 클라이언트에 'STOP' 명령 전송
            send_stop_detection_command()

            # 확정된 품종을 7000번 포트로 C 클라이언트에 전송
            send_confirmed_breed(second_breed)

            # 카운트 초기화
            prediction_count = 0
            prediction_history = np.zeros((5, 4))

        # 추론이 끝난 후 파일 삭제
        os.remove(file_path)
        print(f"Processed and removed image: {file_path}")

    except Exception as e:
        print(f"Error while load image: {e}")
        continue


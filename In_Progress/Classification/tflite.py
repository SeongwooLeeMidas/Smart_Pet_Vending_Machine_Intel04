import tensorflow as tf
from tensorflow.keras.preprocessing import image
import numpy as np
import time

# 1. 원본 모델 로드 (양자화 적용 전)
model = tf.keras.models.load_model('EfficientNetB0-dog-breeds-99.38.h5')

# 2. 모델을 TensorFlow Lite 형식으로 양자화 (INT8 양자화)
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
quantized_model = converter.convert()

# 3. 양자화된 모델을 파일로 저장
with open('quantized_model9.tflite', 'wb') as f:
    f.write(quantized_model)

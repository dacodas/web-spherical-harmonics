from PIL import Image
import numpy as np
from scipy.stats import norm
from scipy.signal import morlet

image_size = 1000
max_gray = 65535
dtype = np.uint16

wavelet = np.real(morlet(image_size))
wavelet = np.interp(wavelet, (wavelet.min(), wavelet.max()), (0, max_gray))
int_array = ( np.repeat([wavelet], image_size, 0) ).astype(dtype)

Image.fromarray(int_array).save("build/wavelet.png")

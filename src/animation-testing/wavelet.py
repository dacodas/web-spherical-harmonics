from PIL import Image
import numpy as np
from scipy.stats import norm
from scipy.signal import morlet

wavelet = np.real(morlet(100))
wavelet = np.interp(wavelet, (wavelet.min(), wavelet.max()), (0, 255))
int_array = ( np.repeat([wavelet], 100, 0) ).astype(np.uint8) 

Image.fromarray(int_array).save("build/wavelet.png")

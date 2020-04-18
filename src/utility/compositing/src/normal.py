from PIL import Image
import numpy as np
from scipy.stats import norm

mean = 0
sigma = .2

# If this had a sigma of 1, how much to get from 1 to 99 percent
x = np.linspace(norm.ppf(0.01), norm.ppf(0.99), 100)

pdf = norm.pdf(x, mean, sigma)
pdf = np.interp(pdf, (pdf.min(), pdf.max()), (0, 255))

int_array = ( np.repeat([pdf], 100, 0) ).astype(np.uint8) 

Image.fromarray(int_array).save("build/normal.png")

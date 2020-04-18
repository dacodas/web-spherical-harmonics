# Overview

# TODO

## Figure out seam 

I think this is due to the normals, but I'm not sure. 

- [ ] Allow for easier swapping of shader programs
- [ ] Try shading without lighting
- [ ] Further debugging...

## Precompile animations

I would like to be able to precompile animations as either a sequence of textures, or as a video. 

### Rotate the texture about in arbitrary axis 

Rotating the texture in 2D doesn't make any sense, let's use the equations from [here](https://math.stackexchange.com/questions/1019910/rotation-matrix-in-spherical-coordinates) and [here](https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula#Matrix_notation). 

I don't think we have to do any conversion, I think we can just do the rotation with the vector components and be just fine. 

### Series of textures

- [X] Generate sequence of textures representing an animation
- [X] For each animation request
  - [X] Load the next textures from the animation
  - [X] Draw
- [ ] Allow toggling between the original vibrate-back-and-forth and the series of textures
- [ ] Also animate and load normals

### From video

#### Some reference

[HTML5 frame-by-frame](https://stackoverflow.com/questions/4298084/html5-frame-by-frame-viewing-frame-seeking)

#### Steps

- [ ] Generate sequence of textures 
- [ ] Compress sequence of textures into video
- [ ] Load up video with Javascript in such a way that I can get an even framerate and access frames individually
- [ ] For each animation request
  - [ ] Load the next frame from the animation
  - [ ] Draw

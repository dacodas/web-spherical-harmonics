// https://webglfundamentals.org/webgl/lessons/webgl-fundamentals.html

function createShader(gl, type, source)
{
    var shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    if (success) {
        return shader;
    }
    
    console.log(gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
}

function createProgram(gl, vertexShader, fragmentShader)
{
    var program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    var success = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (success) {
        return program;
    }
    
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

function createProgramFromScriptSource(gl, vertexShaderID, fragmentShaderID)
{
    var vertexShaderSource = document.getElementById(vertexShaderID).text;
    var fragmentShaderSource = document.getElementById(fragmentShaderID).text;
    
    var vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    var fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    console.log("Creating program " + vertexShaderID);
    program = createProgram(gl, vertexShader, fragmentShader);

    return program 
}

// https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial/Using_textures_in_WebGL
//
// Initialize a texture and load an image.
// When the image finished loading copy it into the texture.
//
function loadRhoTexture(gl, url) {
  const texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);

  // Because images have to be download over the internet
  // they might take a moment until they are ready.
  // Until then put a single pixel in the texture so we can
  // use it immediately. When the image has finished downloading
  // we'll update the texture with the contents of the image.
  const level = 0;
  const internalFormat = gl.RGBA;
  const width = 1;
  const height = 1;
  const border = 0;
  const srcFormat = gl.RGBA;
  const srcType = gl.UNSIGNED_BYTE;
  const pixel = new Uint8Array([0, 0, 255, 255]);  // opaque blue
  gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
                width, height, border, srcFormat, srcType,
                pixel);

  const image = new Image();
  image.onload = function() {
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, srcFormat, srcType, image);

    // WebGL1 has different requirements for power of 2 images
    // vs non power of 2 images so check if the image is a
    // power of 2 in both dimensions.
    if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
       // Yes, it's a power of 2. Generate mips.
       gl.generateMipmap(gl.TEXTURE_2D);
    } else {
       // No, it's not a power of 2. Turn off mips and set
       // wrapping to clamp to edge
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.MIRRORED_REPEAT);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.MIRRORED_REPEAT);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    }
  };
  image.src = url;

  return texture;
}

function loadNewTextures() {

    var texturePaths = JSON.parse(this.response)

    rhoTexture = loadRhoTexture(gl, texturePaths[0])
    normalsTexture = loadRhoTexture(gl, texturePaths[1])
}

function requestNewTextures(texture_components) {
    var textureRequest = new XMLHttpRequest();
    textureRequest.addEventListener("load", loadNewTextures);
    textureRequest.open("GET", "/spherical-harmonics/fcgi-bin/texture-generator");
    textureRequest.setRequestHeader("X-Texture-Components", texture_components)
    textureRequest.send();
}

function requestNewTexturesFromTextarea() {
    requestNewTextures(document.getElementById('new-texture-specification').value.replace("\n", " "));
}

function isPowerOf2(value) {
  return (value & (value - 1)) == 0;
}

var gl;
var positions;
// var eye = {'rho': 5, 'theta': Math.PI / 2, 'phi': 0}
var eye = {'rho': 5, 'theta': 0, 'phi': 0}

var identity4 = glm.mat4(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0, 
                         0, 0, 0, 1)

var rhoTexture;
var textureProgram;
var texture_vertex_buffer;
var texture_element_buffer;

var baseProgram;
var positionBuffer;

function eye_position()
{
    return glm.vec3(
        eye.rho * Math.sin(eye.theta) * Math.cos(eye.phi),
        eye.rho * Math.sin(eye.theta) * Math.sin(eye.phi),
        eye.rho * Math.cos(eye.theta)
    )
}

function initializeTextureBuffers()
{
    // rhoTexture = loadRhoTexture(gl, "test.pgm")
    rhoTexture = loadRhoTexture(gl, "test.png")
    textureProgram = createProgramFromScriptSource(gl, "texture-vertex-shader", "texture-fragment-shader");

    texture_vertex_buffer = gl.createBuffer()
    gl.bindBuffer(gl.ARRAY_BUFFER, texture_vertex_buffer)
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([0, 0, 0, 1, 1, 0, 1, 1]), gl.STATIC_DRAW)

    texture_element_buffer = gl.createBuffer()
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, texture_element_buffer)
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint8Array([0, 1, 2, 3]), gl.STATIC_DRAW)
}

function initializeBaseBuffers()
{
    positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

    var positions = [
        0, 0,
        0, 0.5,
        0.7, 0,
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
}

function initialize()
{
    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    gl = canvas.getContext("webgl");

    // Only continue if WebGL is available and working
    if (gl === null) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
    }

    initializeTextureBuffers()
    initializeBaseBuffers()

    baseProgram = createProgramFromScriptSource(gl, "base-vertex-shader", "base-fragment-shader");
}

function baseVertexAttribute()
{
    gl.useProgram(baseProgram);

    var positionAttributeLocation = gl.getAttribLocation(baseProgram, "a_position");
    gl.enableVertexAttribArray(positionAttributeLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

    // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(positionAttributeLocation, size, type, normalize, stride, offset)
}

function textureVertexAttribute()
{
    gl.useProgram(textureProgram)

    gl.enableVertexAttribArray(0)
    gl.bindBuffer(gl.ARRAY_BUFFER, texture_vertex_buffer);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, texture_element_buffer);

    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(0, size, type, normalize, stride, offset)
}

function render(time)
{
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    eye.rho = 3 + 2 * Math.cos(time * 0.004)
    let view_matrix = glm.lookAt(
        eye_position(),
        glm.vec3(0, 0, 0),
        glm.vec3(0, 1, 0)
    )

    let projection_matrix = glm.perspective( Math.PI / 2, (640 / 480), 0.1, 10000 )

    gl.useProgram(baseProgram)
    baseVertexAttribute()

    var modelUniformLocation = gl.getUniformLocation(baseProgram, "model");
    var viewUniformLocation = gl.getUniformLocation(baseProgram, "view");
    var projUniformLocation = gl.getUniformLocation(baseProgram, "proj");

    gl.uniformMatrix4fv(modelUniformLocation, false, identity4.elements)
    gl.uniformMatrix4fv(viewUniformLocation, false, view_matrix.elements)
    gl.uniformMatrix4fv(projUniformLocation, false, projection_matrix.elements)

    gl.drawArrays(gl.TRIANGLES, 0, 3);

    gl.useProgram(textureProgram);

    // uniform sampler2D rho_sampler;
    // uniform float screen_scale;
    var rhoSamplerUniformLocation = gl.getUniformLocation(textureProgram, "rho_sampler")
    var screenScaleUniformLocation = gl.getUniformLocation(textureProgram, "screen_scale")

    gl.activeTexture(gl.TEXTURE0)
    gl.bindTexture(gl.TEXTURE_2D, rhoTexture)
    gl.uniform1i(rhoSamplerUniformLocation, 0)

    textureVertexAttribute()

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4)
    
    // console.log(time)

    requestAnimationFrame(render)
}

initialize()
requestAnimationFrame(render)

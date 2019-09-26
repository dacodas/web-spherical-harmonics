var gl;
var program;
var positions;
// var eye = {'rho': 5, 'theta': Math.PI / 2, 'phi': 0}
var eye = {'rho': 5, 'theta': 0, 'phi': 0}

var identity4 = glm.mat4(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0, 
                         0, 0, 0, 1)

function initialize()
{
    const canvas = document.querySelector("#glCanvas");
    // Initialize the GL context
    gl = canvas.getContext("webgl");

    // Only continue if WebGL is available and working
    if (gl === null) {
        alert("Unable to initialize WebGL. Your browser or machine may not support it.");
    }
}

function setupShaders()
{
    var vertexShaderSource = document.getElementById("base-vertex-shader").text;
    var fragmentShaderSource = document.getElementById("base-fragment-shader").text;
    
    var vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    var fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    program = createProgram(gl, vertexShader, fragmentShader);
}

function render(time)
{
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    var positionAttributeLocation = gl.getAttribLocation(program, "a_position");
    var positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

    var positions = [
        0, 0,
        // 0, 0.5 + Math.random() * 0.1,
        0, 0.5,
        0.7, 0,
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
    gl.useProgram(program);
    gl.enableVertexAttribArray(positionAttributeLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

    // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(positionAttributeLocation, size, type, normalize, stride, offset)

    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 3;

    eye.rho = 3 + 2 * Math.cos(time * 0.004)

    let eye_position = glm.vec3(
        eye.rho * Math.sin(eye.theta) * Math.cos(eye.phi),
        eye.rho * Math.sin(eye.theta) * Math.sin(eye.phi),
        eye.rho * Math.cos(eye.theta)
    )

    let view_matrix = glm.lookAt(
        eye_position,
        glm.vec3(0, 0, 0),
        glm.vec3(0, 1, 0)
    )

    let projection_matrix = glm.perspective( Math.PI / 2, (640 / 480), 0.1, 10000 )

    var modelUniformLocation = gl.getUniformLocation(program, "model");
    var viewUniformLocation = gl.getUniformLocation(program, "view");
    var projUniformLocation = gl.getUniformLocation(program, "proj");

    gl.uniformMatrix4fv(modelUniformLocation, false, identity4.elements)
    gl.uniformMatrix4fv(viewUniformLocation, false, view_matrix.elements)
    gl.uniformMatrix4fv(projUniformLocation, false, projection_matrix.elements)

    gl.drawArrays(primitiveType, offset, count);
    // console.log(time)
    
    requestAnimationFrame(render)
}

initialize()
setupShaders()

requestAnimationFrame(render)

// function render()
// {
//     gl:clearDepth(1.0)
//     gl.clear(gl.COLOR_BUFFER_BIT)
//     gl.clear(gl.DEPTH_BUFFER_BIT)

    // let eye_position = glm.vec3(
    //     eye.rho * sin(eye.theta) * cos(eye.phi),
    //     eye.rho * sin(eye.theta) * sin(eye.phi),
    //     eye.rho * cos(eye.theta)
    // )

    // let view_matrix = glm.lookAt(
    //     eye_position,
    //     glm.vec3(0, 0, 0),
    //     glm.vec3(0, 0, 1)
    // )

    // let projection_matrix = glm.perspective( Math.PI / 2, (640 / 480), 0.1, 10000 )
// }


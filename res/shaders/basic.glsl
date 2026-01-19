#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 uProjection;
uniform mat4 uModel;

out vec4 vColor;

void main() {
    vColor = aColor;
    gl_Position = uProjection * uModel * vec4(aPos, 1.0);
    // При рендеринге точек может понадобиться gl_PointSize
    gl_PointSize = 5.0; 
}

#type fragment
#version 330 core
in vec4 vColor;
out vec4 FragColor;

void main() {
    FragColor = vColor;
}

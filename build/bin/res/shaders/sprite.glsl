#type vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uProjection;
uniform mat4 uModel;

out vec2 vTexCoord;

void main() { 
    vTexCoord = aTexCoord;
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0); 
}

#type fragment
#version 330 core
out vec4 FragColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;

void main() { 
    FragColor = texture(uTexture, vTexCoord); 
}

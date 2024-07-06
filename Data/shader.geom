#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

noperspective out vec3 distanceEdge;
out vec3 fragPos;
out vec3 normal;

uniform mat4 u_mvpMatrix;

const vec3 lightPos = vec3(100.0f,100.0f,100.0f);
out vec3 lightPosMvp;

void main() {
    vec2 scale = vec2(500.0f, 500.0f);
    vec2 p0 = scale * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = scale * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = scale * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;

    vec2 v0 = p2-p1;
    vec2 v1 = p2-p0;
    vec2 v2 = p1-p0;
    float area = abs(v1.x*v2.y - v1.y*v2.x);

    normal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                             gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
    
    lightPosMvp = (u_mvpMatrix * vec4(lightPos,0.0)).xyz;
    
    distanceEdge = vec3(area/length(v0),0,0);
    fragPos = gl_in[0].gl_Position.xyz;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    distanceEdge = vec3(0,area/length(v1),0);
    fragPos = gl_in[1].gl_Position.xyz;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    distanceEdge = vec3(0,0,area/length(v2));
    fragPos = gl_in[2].gl_Position.xyz;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}

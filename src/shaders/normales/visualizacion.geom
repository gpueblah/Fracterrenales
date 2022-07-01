#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;
uniform bool showMidpoint;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    if (showMidpoint) {
        vec4 f = projection * gl_in[0].gl_Position;
        vec4 s = projection * gl_in[1].gl_Position;
        vec4 t = projection * gl_in[2].gl_Position;

        vec4 m = (f + s + t) / 3;
        gl_Position = m;
        EmitVertex();

        vec4 nf = projection * (gl_in[0].gl_Position + vec4(gs_in[0].normal, 0.0) * MAGNITUDE);
        vec4 ns = projection * (gl_in[1].gl_Position + vec4(gs_in[1].normal, 0.0) * MAGNITUDE);
        vec4 nt = projection * (gl_in[2].gl_Position + vec4(gs_in[2].normal, 0.0) * MAGNITUDE);

        vec4 nm = (nf + ns + nt) / 3;
        gl_Position = nm;
        EmitVertex();
        EndPrimitive();
    } else {
        GenerateLine(0); // first vertex normal
        GenerateLine(1); // second vertex normal
        GenerateLine(2); // third vertex normal
    }
}
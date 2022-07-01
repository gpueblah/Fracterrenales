#version 330 core
out vec4 FragColor;

out float sh;

// Según Unity
// Subtract a from both a and b and value to make a', b' and value'.
// This makes a' to be zero and b' and value' to be reduced. Finally divide value' by b'. This gives the InverseLerp amount
float inverselerp(float a, float b, float value) {
    float ap = a - a;
    float bp = b - a;
    float valuep = value - a;
    return valuep / bp;
}

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}


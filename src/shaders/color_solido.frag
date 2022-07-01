#version 330 core

uniform vec3 color;
uniform bool colorDePendiente;

out vec4 FragColor;

in vec3 normal;
in vec3 pos;

void main()
{
    if (colorDePendiente) {
        // Sería
        // float d = dot(vec3(0.0, 1.0, 0.0), normal);
        // pero normal está, pues, normalizado. normal.y es una buena aproximacion de la pendiente
        FragColor = vec4(0.0, normal.y, 1.0 - normal.y, 1.0);
    } else if (false) {
        // GMOD
        float xint = 0.0;
        float xfrac = modf(pos.x, xint);
        float zint = 0.0;
        float zfrac = modf(pos.z, zint);

        xfrac = xfrac + 0.5;
        bool celda = mod(xint + zint, 2) == 0;
        if (celda) {
            FragColor = vec4(1.0, 0.0, 0.86, 1.0);
        } else {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }

        //FragColor = vec4(xfrac, 0.0, zfrac, 1.0);
    } else {
        FragColor = vec4(color, 1.0);
    }
}
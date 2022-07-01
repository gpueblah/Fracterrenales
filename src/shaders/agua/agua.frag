#version 330 core
out vec4 FragColor;

in vec4 Position;
in vec2 TexCoords;
in vec3 FragPos;

uniform vec3 color;
uniform float alpha;
uniform float near_plane;
uniform float far_plane;


uniform float time;

uniform sampler2D agua;
uniform sampler2D flow;
uniform sampler2D normal;

uniform float tiling;
uniform float speed;
uniform float strength;

uniform vec3 viewPos;

uniform int tipoNiebla;
uniform vec3 FogColor;

uniform float FogMax;
uniform float FogMin;

uniform float FogDensity;

uniform vec3 lightColor;

uniform vec3 lightPos;

uniform bool iluminar;


float getLinearFogFactor(float c) {
    if (c>=FogMax) return 1;
    if (c<=FogMin) return 0;

    return 1 - (FogMax - c) / (FogMax - FogMin);
}

float getExpFogFactor(float c) {   
    const float e = 2.71828;

    return 1 - pow(e, -FogDensity * c);
}

float getExp2FogFactor(float c) {
    const float e = 2.71828;

    return 1 - pow(e, -FogDensity * pow(c, 2));
}

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

vec3 FlowUV(vec2 uv, vec2 flowVector, float t, float offset) {
    float progress = fract(t + offset);
    vec2 xy = (uv - flowVector * progress) * tiling + offset;
    return vec3(xy, 1 - abs(1 - 2 * progress));
}


vec3 UnpackNormal(vec3 packedNormal) {
    return packedNormal.rbg * 2 - 1;
}

void main()
{
    // vec3 ndc = Position.xyz / Position.w; //perspective divide/normalize
    // vec2 uv = ndc.xy * 0.5 + 0.5; //ndc is -1 to 1 in GL. scale for 0 to 1
    // FragColor = vec4(ColorBelowWater(), alpha);
    // FragColor = vec4(vec3(0.3, 0.1, 0.8), alpha);
    vec4 flowTexel = texture(flow, TexCoords);
    vec2 flowVector = (flowTexel.rg * 2 - 1) * strength;
    float t = time * speed + flowTexel.a;

    vec3 uv = FlowUV(TexCoords, flowVector, t, 0.0);
    vec3 col = vec3(texture(agua, uv.xy).r) * uv.z;

    vec3 uvp = FlowUV(TexCoords, flowVector, t, 0.5);
    vec3 colp = vec3(texture(agua, uvp.xy + vec2(0.5)).r) * uvp.z;


    vec3 n1 = UnpackNormal(vec3(texture(normal, uv.xy))) * uv.z;
    vec3 n2 = UnpackNormal(vec3(texture(normal, uvp.xy + vec2(0.5)))) * uvp.z;

    vec3 normal = normalize(n1 + n2);


    vec4 color_agua = vec4((col + colp) * color, alpha);
    

    vec4 lighting = color_agua;
    if (iluminar) {
        vec3 lightColor = lightColor;
        // ambient
        vec3 ambient = 0.3 * lightColor;

        // diffuse
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * lightColor;

        // specular
        float shinyness = 32;
        float specStrength = 0.6578;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shinyness);
        vec3 specular = spec * lightColor * specStrength;
                   
        lighting = vec4((ambient + diffuse + specular) * color_agua.rgb, color_agua.a);
    }
       
	float nieblaAlpha = 0.0;
    switch (tipoNiebla) {
        case 1: {
            nieblaAlpha = getLinearFogFactor(distance(viewPos, FragPos));
            break;
        }
        case 2: {
            nieblaAlpha = getExpFogFactor(distance(viewPos, FragPos));
            break;
        }
        case 3: {
            nieblaAlpha = getExp2FogFactor(distance(viewPos, FragPos));
            break;
        }
    }
    vec4 color_niebla = vec4(FogColor, 1.0);

    FragColor = mix(lighting, color_niebla, nieblaAlpha);
    gl_FragDepth = gl_FragCoord.z;
}

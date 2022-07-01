#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform bool iluminar;

uniform sampler2D shadowMap;

uniform float minHeight;
uniform float maxHeight;

uniform vec3 l1Color;
uniform vec3 l2Color;
uniform vec3 l3Color;
uniform vec3 l4Color;

uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int tipoNiebla;
uniform vec3 FogColor;

uniform float FogMax;
uniform float FogMin;

uniform float FogDensity;

uniform float q1;

uniform int pcf;


float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float count = 0;
    for(int x = -pcf; x <= pcf; ++x)
    {
        for(int y = -pcf; y <= pcf; ++y)
        {
            count += 1;
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= count;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

// Según Unity
// Subtract a from both a and b and value to make a', b' and value'.
// This makes a' to be zero and b' and value' to be reduced. Finally divide value' by b'. This gives the InverseLerp amount
float inverselerp(float a, float b, float value) {
    float ap = a - a;
    float bp = b - a;
    float valuep = value - a;
    return valuep / bp;
}

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

void main() {
    vec3 color = vec3(0.0);
    float specStrength = 0.25;
    float shinyness = 32;
    float s = inverselerp(minHeight, maxHeight, fs_in.FragPos.y);
    float q1s = inverselerp(minHeight, maxHeight, q1-0.1);
    float q1ss = inverselerp(minHeight, maxHeight, q1);

    if (s <= q1s) {
        color = l1Color;
        specStrength = 0.75;
        //shinyness = 32;
    } else if(s <= max(q1ss, 0.4)) {
        color = mix(l1Color, l2Color, smoothstep(q1s, max(q1ss, 0.4), s));
    } else if (s <= 0.45) {
        color = l2Color;
    } else if (s <= 0.55) {
        color = mix(l2Color, l3Color, smoothstep(0.45, 0.55, s));
    } else if (s <= 0.7) {
        color = l3Color;
    } else if (s <= 0.8) {
        color = mix(l3Color, l4Color, smoothstep(0.7, 0.8, s));
        specStrength = 0.85;
        //shinyness = 32;
    } else {
        specStrength = 1;
        //shinyness = 64;
        color = l4Color;
    }
    vec3 lighting = color;

    if (iluminar) {
        vec3 normal = normalize(fs_in.Normal);
        vec3 lightColor = lightColor;
        // ambient
        vec3 ambient = 0.3 * lightColor;

        // diffuse
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * lightColor;

        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shinyness);
        vec3 specular = spec * lightColor * specStrength;

        // calculate shadow
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
        lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    }

        
    
    float alpha = 0.0;
    switch (tipoNiebla) {
        case 1: {
            alpha = getLinearFogFactor(distance(viewPos, fs_in.FragPos));
            break;
        }
        case 2: {
            alpha = getExpFogFactor(distance(viewPos, fs_in.FragPos));
            break;
        }
        case 3: {
            alpha = getExp2FogFactor(distance(viewPos, fs_in.FragPos));
            break;
        }
    }

    FragColor = vec4(mix(lighting, FogColor, alpha), 1.0f);
}
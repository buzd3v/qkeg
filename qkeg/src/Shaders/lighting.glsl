#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

// Adopted from: https://google.github.io/filament/Filament.md.html
#define PI 3.1415926535897932384626433832795

// Trowbridge-Reitz GGX
float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

// Fresnel-Schlick approximation
vec3 F_Schlick(float HoV, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - HoV, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}


float Fd_Lambert() {
    return 1.0 / PI;
}

vec3 pbrBRDF(
        vec3 diffuse, float roughness, float metallic, vec3 f0,
        vec3 n, vec3 v, vec3 l, vec3 h, float NoL)
{
    float NoV = abs(dot(n, v));
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float HoV = clamp(dot(h, v), 0.0, 1.0);

    float D = D_GGX(NoH, roughness);
    vec3  F = F_Schlick(HoV, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 Fr = (D * V) * F;

    // diffuse BRDF
    vec3 Fd = diffuse * Fd_Lambert();

    return Fd + Fr;
}

float calculateSpecularBP(float NoH) {
    float shininess = 32.0 * 4.0;
    return pow(NoH, shininess);
}

vec3 blinnPhongBRDF(vec3 diffuse, vec3 n, vec3 v, vec3 l, vec3 h) {
    vec3 Fd = diffuse;

    // specular
    // TODO: read from spec texture / pass spec param
    vec3 specularColor = diffuse * 0.5;
    float NoH = clamp(dot(n, h), 0, 1);
    vec3 Fr = specularColor * calculateSpecularBP(NoH);

    return Fd + Fr;
}

float calculateDistanceAttenuation(float dist, float range)
{
    float d = clamp(1.0 - pow((dist/range), 4.0), 0.0, 1.0);
    return d / (dist*dist);
}

// See KHR_lights_punctual spec - formulas are taken from it
float calculateAngularAttenuation(
        vec3 lightDir, vec3 l,
        vec2 scaleOffset) {
    float cd = dot(lightDir, l);
    float angularAttenuation = clamp(cd * scaleOffset.x + scaleOffset.y, 0.0, 1.0);
    angularAttenuation *= angularAttenuation;
    // angularAttenuation = smoothstep(0, 1, angularAttenuation);
    return angularAttenuation;
}

float calculateAttenuation(vec3 pos, vec3 l, Light light) {
    float dist = length(light.position - pos);
    float atten = calculateDistanceAttenuation(dist, light.range);
    if (light.type == TYPE_LIGHT_SPOT) {
        atten *= calculateAngularAttenuation(light.direction, l, light.scaleOffset);
    }
    return atten;
}

vec3 calculateLight(Light light, vec3 fragPos, vec3 n, vec3 v, vec3 l,
        vec3 diffuseColor, float roughness, float metallic, vec3 f0, float occlusion) 
{
    vec3 h = normalize(v + l);
    float NoL = clamp(dot(n, l), 0.0, 1.0);

    float atten = 1.0;
    if (light.type != TYPE_LIGHT_DIRECTIONAL) {
        atten = calculateAttenuation(fragPos, l, light);
    }

#ifdef PBR
    vec3 fr = pbrBRDF(
            diffuseColor, roughness, metallic, f0,
            n, v, l, h, NoL);
#else
    vec3 fr = blinnPhongBRDF(diffuseColor, n, v, l, h);
#endif
    return (fr * light.color) * (light.intensity * atten * NoL * occlusion);
}

#endif
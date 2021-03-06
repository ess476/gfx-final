#version 330 core

in vec3 vertex;                 // The position of the vertex, in camera space
in vec3 vertexToLight;          // Vector from the vertex to the light
in vec3 vertexToEye;            // Vector from the vertex to the eye
in vec3 eyeNormal;		// Normal of the vertex, in camera space

uniform samplerCube envMap;	// The cube map containing the environment to reflect
uniform vec4 ambient;		// The ambient channel of the color to reflect
uniform vec4 diffuse;		// The diffuse channel of the color to reflect
uniform vec4 specular;		// The specular channel of the color to reflect

uniform mat4 model;             // model matrix
uniform mat4 view;              // view matrix
uniform mat4 mvp;               // model view projection matrix

uniform float r0;		// The Fresnel reflectivity when the incident angle is 0
uniform float m;		// The roughness of the material

out vec4 fragColor;

void main()
{
    vec3 n = normalize(eyeNormal);
    vec3 l = normalize(vertexToLight);
    vec3 cameraToVertex = normalize(vertex); //remember we are in camera space!

    //TODO: fill the rest in
    vec4 i = inverse(view) * vec4(reflect(cameraToVertex, n), 0);


    vec4 reflectionColor = texture(envMap, i.xyz);

    vec3 u = vertexToLight;
    vec3 v = vertexToEye;

    vec3 h = normalize(u + v);

    float alpha = acos(dot(n, h));
    float d = exp(-(pow(tan(alpha), 2)) / pow(m, 2)) / (4 * pow(m, 2) * pow(cos(alpha), 4));

    float common_term = 2*dot(h, n)/dot(v, h);

    float g = min(1, min(common_term * dot(v, n), common_term * dot(u, n)));

    float f = r0 + (1 - r0)*pow(1 - cos(dot(n, cameraToVertex)), 5);

    float k = max(0, (d * f * g) / dot(v, n));


    // Add diffuse component
    float lambert = max(0.0, dot(n, l));

    vec4 objColor = ambient + lambert * diffuse + k * specular;

    fragColor = mix(objColor, reflectionColor, f);
}

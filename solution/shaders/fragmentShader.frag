#version 420 core
in vec2 UV;
in vec3 position;
in vec3 normal;

out vec3 FragColor;

uniform sampler2D image;
uniform int lights = 1;
uniform vec3 lightPositions;
uniform float lightIntensities;
uniform vec3 cameraPosition;
uniform float reflectivity = 1.0f;
uniform float shinyness = 32.0f;
  
void main()
{
    float ambient = 0.6;
    vec3 cameraDirection = cameraPosition - position;

    vec3 intensity = ambient * vec3(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < lights; ++i) {
        vec3 lightDirection = lightPositions[i] - position;
        float lightDistance = length(lightDirection);

        float costheta = max( dot( normalize(normal), normalize(lightDirection) ), 0.0f );
        float diffuse = reflectivity * lightIntensities * costheta;

        float cosphi = max( dot( normalize(lightDirection), normalize(cameraDirection) ), 0.0f );
        float specular = reflectivity * lightIntensities * pow(cosphi, shinyness);

        float lightsIntensity = (diffuse + specular) * (10 / pow(lightDistance, 2));
        intensity = intensity + (lightsIntensity * vec3(1.0f, 1.0f, 1.0f));
    }

    FragColor = intensity * texture(image, UV).rgb;
}





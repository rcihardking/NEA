#version 420 core
in vec2 UV;
in vec3 position;
in vec3 normal;

out vec3 FragColor;

uniform sampler2D image;
uniform vec3 lightPosition;
  
void main()
{
    //FragColor = vec3(0.5f, 0.5f, 0.5f);
    vec3 ambient = 0.6 * vec3(1.0f, 1.0f, 1.0f);

    vec3 unitNormal = normalize(normal);
    vec3 lightDirection = lightPosition - position;
    float lightDistance = length(lightDirection);
    float impact = max(dot(unitNormal, normalize(lightDirection)), 0.0);
    vec3 diffuse = impact * (10/lightDistance) * vec3(1.0f, 1.0f, 1.0f);

    FragColor = (ambient + diffuse) * texture(image, UV).rgb;
}





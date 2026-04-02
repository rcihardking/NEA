#version 420 core
in vec2 UV;
in vec3 position;
in vec3 normal;

out vec3 FragColor; //sends the colour of the fragment to opengl

//variables from the program
uniform sampler2D image;
uniform int lights = 1;
uniform vec3 lightPositions;
uniform float lightIntensities;
uniform vec3 cameraPosition;
uniform float reflectivity = 1.0f;
uniform float shinyness = 32.0f;
uniform float ambient;
  
void main()
{
    vec3 cameraDirection = cameraPosition - position;  

    vec3 intensity = reflectivity * ambient * vec3(1.0f, 1.0f, 1.0f);   //calculate inital ambient light intensity

    for (int i = 0; i < lights; ++i) {  //iterate through all lights
        vec3 lightDirection = lightPositions[i] - position;
        float lightDistance = length(lightDirection);

        //calculate diffuse intensity
        float costheta = max( dot( normalize(normal), normalize(lightDirection) ), 0.0f );
        float diffuse = reflectivity * lightIntensities * costheta;

        //calculate specular intensity
        float cosphi = max( dot( normalize(lightDirection), normalize(cameraDirection) ), 0.0f );
        float specular = reflectivity * lightIntensities * pow(cosphi, shinyness);

        //calculate total intensity
        float lightsIntensity = (diffuse + specular) * (10 / pow(lightDistance, 2));
        intensity = intensity + (lightsIntensity * vec3(1.0f, 1.0f, 1.0f));
    }

    FragColor = intensity * texture(image, UV).rgb; //put texture and light onto fragment
}





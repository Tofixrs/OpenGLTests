#version 330 core
out vec4 FragColor;
in vec2 TexCord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse[8];
uniform sampler2D texture_specular[8];
uniform int diff_txt_number;
uniform int specular_txt_number;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
float specularStrength = 0.5;

void main()
{
  vec4 texelColor = vec4(1.0);
  for(int i = 0; i < diff_txt_number; i++) {
      texelColor *= texture(texture_diffuse[i], TexCord);
  }
  for(int i = 0; i < specular_txt_number; i++) {
      texelColor *= texture(texture_specular[i], TexCord);
  }

  vec3 ambient = ambientStrength * lightColor;

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;


  FragColor = vec4((ambient + diffuse), 1.0) * texelColor + vec4(specular, 1.0);
}
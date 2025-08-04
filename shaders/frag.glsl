#version 330 core

struct Light {
  vec3 pos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
struct Material {
  sampler2D texture_diffuse[8];
  sampler2D texture_specular[8];
  int diff_number;
  int spec_number;
  float shininess;
};

out vec4 FragColor;
in vec2 TexCord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
  vec4 texelColor = vec4(1.0);
  for(int i = 0; i < material.diff_number; i++) {
      texelColor *= texture(material.texture_diffuse[i], TexCord);
  }
  vec4 specColor = vec4(1.0);
  for(int i = 0; i < material.spec_number; i++) {
      specColor *= texture(material.texture_specular[i], TexCord);
  }
  vec3 ambient = light.ambient * vec3(texelColor);

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.pos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texelColor);



  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * vec3(specColor);

  FragColor = vec4(ambient+diffuse+specular,1.0);
}

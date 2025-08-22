#version 330 core

struct PointLight {
  vec3 pos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
struct Material {
  sampler2D texture_diffuse[8];
  sampler2D texture_specular[8];
  int diff_number;
  int spec_number;
  float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 frag_color;
in vec2 tex_cord;
in vec3 normal;
in vec3 frag_pos;

uniform vec3 view_pos;
uniform Material material;
uniform PointLight point_lights[64];
uniform DirLight dir_light;
uniform int point_light_num;

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 diff_texture, vec3 spec_texture);
vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir, vec3 diff_texture, vec3 spec_texture);


void main() {
  vec4 diff_texture = vec4(1.0);
  for(int i = 0; i < material.diff_number; i++) {
      diff_texture *= texture(material.texture_diffuse[i], tex_cord);
  }
  vec4 spec_texture = vec4(1.0);
  for(int i = 0; i < material.spec_number; i++) {
      spec_texture *= texture(material.texture_specular[i], tex_cord);
  }

  vec3 norm = normalize(normal);
  vec3 view_dir = normalize(view_pos - frag_pos);

	vec3 res = calc_dir_light(dir_light, norm, view_dir, vec3(diff_texture), vec3(spec_texture));

	for (int i = 0; i < point_light_num; i++) {
		res += calc_point_light(point_lights[i], normal, frag_pos, view_dir, vec3(diff_texture), vec3(spec_texture));
	}

  frag_color = vec4(res,1.0);
}


vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 diff_texture, vec3 spec_texture) {
	vec3 light_dir = normalize(light.pos - frag_pos);
	float diff = max(dot(normal, light_dir),0.0);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	float distance = length(light.pos - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * diff_texture;
	vec3 diffuse = light.diffuse * diff * diff_texture;
	vec3 specular = light.specular * spec * spec_texture;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir, vec3 diff_texture, vec3 spec_texture) {
	vec3 light_dir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * diff_texture;
	vec3 diffuse = light.diffuse * diff * diff_texture;
	vec3 specular = light.specular * spec * diff_texture;
	return (ambient + diffuse + specular);
}

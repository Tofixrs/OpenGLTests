#version 330 core
out vec4 FragColor;
in vec2 TexCord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
  vec4 texel1 = texture(texture1, TexCord);
  vec4 texel2 = texture(texture2, TexCord);

  FragColor = mix(
    texel2,
    texel1,
    texel1.a
  );
}

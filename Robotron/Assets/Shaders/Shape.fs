// Fragment shader

#version 430 core

in vec3 outColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D Texture;
uniform sampler2D Texture2;
uniform float currentTime;

void main()
{
//   vec3 colorTemp = outColor * abs(sin(currentTime));
//
//   color = vec4(colorTemp, 1.0f);
//
// //  color = vec4(outColor, 1.0f);
// 
//   texture(Texture2, TexCoord), 0.2);

//    color = texture(Texture, TexCoord) * vec4(outColor, 1.0f); // * abs(sin(currentTime));

    // Mix of colour and texture
//    color = mix(texture(Texture, TexCoord), texture(Texture2, TexCoord), 0.2) * vec4(outColor, 1.0f);// * abs(sin(currentTime));  

    // Mix of 2 textures only
    color = mix(texture(Texture, TexCoord), texture(Texture2, TexCoord), 0.5);// * vec4(outColor, 1.0f);// * abs(sin(currentTime));  

}
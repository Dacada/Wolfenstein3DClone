in vec2 texCoord0;
out vec4 fragColor;

uniform vec3 color;
uniform sampler2D sampler;

void main() {
	fragColor = texture2D(sampler, texCoord0.xy);
	
	if (fragColor == 0)
		fragColor = vec4(color, 1);
	else
		fragColor *= vec4(color, 1);
}
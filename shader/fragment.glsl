#version 460

out vec4 FragColor;

void main() {
	vec3 color = vec3(0.1, 0.2, 0.3);
	for(int i=0; i<10000; ++i) {
		color = cos(color + float(i) * 0.02);
		color = abs(sin(color));
	}
	FragColor = vec4(color, 1.0);
}

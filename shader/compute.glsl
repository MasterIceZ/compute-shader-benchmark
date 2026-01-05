#version 460

layout(local_size_x = 16, local_size_y = 16) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main() {
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec3 color = vec3(0.1, 0.2, 0.3);

	for(int i=0; i<10000; ++i) {
		color = cos(color + float(i) * 0.02);
		color = abs(sin(color));
	}
	imageStore(imgOutput, texelCoord, vec4(color, 1.0));
}

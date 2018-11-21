#version 330
// fragment shader for phong-model lighting with a single directional light source.
// TODO: extend this shader to
//  - support diffuse textures
//  - support shadow mapping

in vec4 var_Color;
in vec3 var_Normal;
in vec3 var_Position;

uniform vec3 camPos;

uniform vec3 diffColor;
uniform vec3 specColor;
uniform vec3 ambientColor;
uniform float shininess;
uniform float alpha;

// TODO add more uniforms

uniform vec3 lightPos;
uniform vec3 lightDiff;
uniform sampler2D diffuseTex;
uniform sampler2D shadowTex;
uniform mat4 light_VP;

layout(location=0) out vec4 out_Color;

vec4 blinn_phong(vec3 kd) {
    // Implement Blinn-Phong Shading Model
    // 1. Convert everything to world space
    //    and normalize directions
    vec4 pos_world = vec4(var_Position, 1);
    vec3 normal_world = normalize(var_Normal);
    pos_world /= pos_world.w;
    vec3 light_dir = normalize(lightPos);
    vec3 cam_dir = camPos - pos_world.xyz;
    cam_dir = normalize(cam_dir);

    // 2. Compute Diffuse Contribution
    float ndotl = max(dot(normal_world, light_dir), 0.0);
    vec3 diffContrib = lightDiff * kd * ndotl;

    // 3. Compute Specular Contribution
    vec3 R = reflect( -light_dir, normal_world );
    float eyedotr = max(dot(cam_dir, R), 0.0);
    vec3 specContrib = pow(eyedotr, shininess) *
                       specColor * lightDiff;

    // 4. Add specular and diffuse contributions
    return  + vec4(diffContrib + specContrib, alpha);
}

void main () {
	// TODO implement texture mapping here
	// TODO implement shadow mapping here
	vec4 x_world = vec4(var_Position, 1);

	vec4 x_ndc = light_VP * x_world;
	vec4 x_tex = (x_ndc + vec4(1.0, 1.0, 1.0, 1.0)) / 2.0;

    float occluder_depth = texture(shadowTex, x_tex.xy).r;
	float this_depth = x_tex.z;
	float bias = 0.01;
	
	vec3 kd = texture(diffuseTex, var_Color.xy).xyz;
	if (occluder_depth + bias < this_depth) {
		out_Color += vec4(ambientColor + blinn_phong(kd).xyz, 1) * 0.7;
	} 
	else {
		out_Color += vec4(ambientColor + blinn_phong(kd).xyz, 1);
	}
}

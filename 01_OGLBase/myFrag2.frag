#version 140

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// irány fényforrás: fény iránya
uniform vec3 light_dir = vec3(-1f,-1f,-1f);

uniform vec3 eye_pos;

// fénytulajdonságok: ambiens, diffúz, ...
uniform vec3 La = vec3(0.4, 0.4, 0.4);  //ambient
uniform vec3 Ld = vec3(0.6, 0.6, 0.6);  //difusse
uniform vec3 Ls = vec3(0.8, 0.8, 0.8);  //specular

uniform sampler2D texImage;

// anyagtulajdonságok: ambiens, diffúz, ...
uniform vec3 Ka = vec3(1, 1, 1);
uniform vec3 Kd = vec3(1, 1, 1);
uniform vec3 Ks = vec3(1, 1, 1);

//uniform sampler2D texImage;

void main()
{	
	// ambiens szín számítása
	vec3 ambient = La; //* Ka;

	// diffúz szín számítása
	vec3 n = normalize(vs_out_norm);
	vec3 l = -normalize(light_dir);
	float di = clamp(dot(n, l), 0, 1);
	vec3 diffuse = Ld * di;// * Kd;

	vec3 r = reflect(normalize(light_dir), n);
	vec3 c = normalize(eye_pos - vs_out_pos);
	float si = clamp(dot(r, c), 0, 1);
	vec3 specular = Ls * si;// * Ks ;
	
	// a fragment végsõ színének meghatározása
	//fs_out_col = vec4(ambient + diffuse + specular, 1);

	// felületi normális
	//fs_out_col = vec4(vs_out_norm, 1);
	//fs_out_col = vec4(ambient + diffuse + specular, 1);

	//textúrával
	vec4 textureColor = texture(texImage, vs_out_tex);
	fs_out_col = vec4(ambient + diffuse + specular, 1) * textureColor;
}

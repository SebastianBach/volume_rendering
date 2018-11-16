#version 410

layout(location = 0) out vec4 vFragColor;	//fragment shader output

smooth in vec4 WorldSpacePos;

uniform vec3 camPos;

uniform float time;

uniform sampler2D	noiseTexture;

uniform int sphereMode;

uniform int renderMode;

//---------------------------------------------------------------------------
// object mode
// 0: spheres
// 1: metaballs
// 2: defomred spheres
//---------------------------------------------------------------------------
uniform int u_objectMode;

//---------------------------------------------------------------------------
/// Animation time value.
//---------------------------------------------------------------------------
uniform float u_animation;

//---------------------------------------------------------------------------
/// Shading mode.
//---------------------------------------------------------------------------
uniform int u_shadingMode;

// user controlled object
uniform float u_dynX;
uniform float u_dynY;



uniform vec3 u_spheres[3];

//---------------------------------------------------------------------------
/// Returns the vector to the light source.
//---------------------------------------------------------------------------
vec3 GetLightDir()
{
	return normalize(vec3(-0.2,1,1.0));
}

const int MODE_SPHERE = 1;
const int MODE_METABALL = 2;
const int MODE_DEFORMED_SPHERE = 3;


// error codes for SampleResult::_error
const int ERROR_NONE = 0;
const int ERROR_UNKNOWN = 1;
const int ERROR_ILLEGALMODE = 2;


const float CONST_PI = 3.1415;
const float CONST_TAU = 6.2831;

//---------------------------------------------------------------------------
/// Structure storing data from sampling space with SampleSpace().
//---------------------------------------------------------------------------
struct SampleGlobalResult
{
	bool _inside;
	vec3 _normal;
	vec3 _color;
	bool _hasUVs;
	vec2 _uvs;
	int _error;	// error code; 
	vec3 _pos;
};


//---------------------------------------------------------------------------
/// Animation Utlity
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Returns an animated value in the range [0, 1].
//---------------------------------------------------------------------------
float GetAnimation01(float timeFactor)
{
	return (sin(u_animation * timeFactor) + 1.0) * .5;
}

//---------------------------------------------------------------------------
/// Returns an animated value in the range [-1, 1].
//---------------------------------------------------------------------------
float GetAnimation11(float timeFactor)
{
	return sin(u_animation * timeFactor);
}

//---------------------------------------------------------------------------
/// Returns the coordinates of the user-controlled object.
//---------------------------------------------------------------------------
vec3 GetDynamicObject()
{
	return vec3(u_dynX, u_dynY, -1.0);
}



// ----------------------------------------------------------------------
/// SPHERE MODE
// ----------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Returns uv-coordinates for the given vector.
//---------------------------------------------------------------------------
vec2 GetUVsFromNormal(vec3 normal)
{
	// https://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere
	vec3 d = - normalize(normal);
	float u = 0.5 + atan(d.z, d.x) / CONST_TAU;
	float v = 0.5 - asin(d.y) / CONST_PI;

	return vec2(u,v);
}

struct SphereSettings
{
	vec3 _center;
	float _radius;
	vec3 _color;
};

//---------------------------------------------------------------------------
/// Checks if the given point is within the given sphere.
//---------------------------------------------------------------------------
SampleGlobalResult CheckGlobalSphere(vec3 pos, SphereSettings settings)
{
	SampleGlobalResult res;
	res._inside = false;

	vec3 dVec = pos - settings._center;
	float dist = length(dVec);

	if(dist <= settings._radius)
	{
		res._inside = true;
		res._normal = normalize(dVec);
		res._color = settings._color;
		res._hasUVs = true;
		res._uvs = GetUVsFromNormal(res._normal);	
		res._pos = pos;
	}

	return res;
}

//---------------------------------------------------------------------------
/// Returns the settings of the sphere with the given index.
//---------------------------------------------------------------------------
SphereSettings GetSphere(int i)
{
	SphereSettings settings;

	if(i == 0)
	{
		settings._radius = 0.5;
		vec3 spherePos = vec3(0.0,0.5,-1.0);
		spherePos.x = GetAnimation11(2.0);
		settings._center = spherePos;

	} 

	if(i == 1)
	{
		settings._radius = 0.25;
		settings._center = GetDynamicObject();
	}

	return settings;
}

//---------------------------------------------------------------------------
/// Samples space for spheres.
//---------------------------------------------------------------------------
SampleGlobalResult SampleSphereMode(vec3 worldPos)
{
	SampleGlobalResult res;
	res._inside = false;
	res._error = ERROR_NONE;

	for(int i = 0; i < 2; ++i)
	{
		SphereSettings settings = GetSphere(i);

		SampleGlobalResult datata = CheckGlobalSphere(worldPos, settings);

		if(datata._inside == true)
		{
			res = datata;
			break;
		}
	}

	return res;
}

// ----------------------------------------------------------------------
/// METABALLS
// ----------------------------------------------------------------------

/// Returned by GetMetaballSettings()
struct MetaballSettings
{
	vec3 _center;
	vec3 _color;
};

//---------------------------------------------------------------------------
/// Returns the settings of the metaball with the given index.
//---------------------------------------------------------------------------
MetaballSettings GetMetaballSettings(int i)
{
	MetaballSettings settings;

	if(i == 0)
	{
		//float period = GetAnimation11(1.0);
		//vec3 pos1 = vec3(-1*period,0.6,- 1.0);
		settings._center = u_spheres[0];
		settings._color = vec3(1.0,0.0,0.0);
	}

	if(i == 1)
	{
		//float period = GetAnimation11(1.0);
		//vec3 pos2 = vec3(1*period,0.3, -1.0);
		settings._center = u_spheres[1];
		settings._color = vec3(0.0,1.0,0.0);
	}

	if(i == 2)
	{
		float period = GetAnimation11(7.0);
		vec3 pos2 = vec3(-1.5*period,0.7, -1.0);
		settings._center = u_spheres[2];
		settings._color = vec3(1.0,1.0,0.0);
	}

	if(i == 3)
	{
		float period = GetAnimation11(5.0);
		vec3 pos2 = vec3(1.5*period,0.3, -1.0);
		settings._center = pos2;
		settings._color = vec3(1.0,0.0,1.0);
	}

	if(i == 4)
	{
		settings._center = GetDynamicObject();
		settings._color = vec3(0.0,0.0,1.0);
	}

	return settings;
}

//---------------------------------------------------------------------------
/// Metaball function.
//---------------------------------------------------------------------------
float MetaballFunction(vec3 pos, vec3 center)
{
	// https://en.wikipedia.org/wiki/Metaballs
	
	float x = pow(pos.x - center.x, 2);
	float y = pow(pos.y - center.y, 2);
	float z = pow(pos.z - center.z, 2);
	float sum = x + y + z;
	float factor = 1 / sum;
	
	return factor;
	
}

struct MetaballFieldSample
{
	float _value;
	vec3 _color;
};

//---------------------------------------------------------------------------
/// Samples the world metaball field.
//---------------------------------------------------------------------------
MetaballFieldSample MetaballField(vec3 pos, bool color)
{
	MetaballFieldSample fieldSample;
	fieldSample._value = 0.0;
	fieldSample._color = vec3(0.0);

	for(int i = 0; i < 5; ++i)
	{
		MetaballSettings settings = GetMetaballSettings(i);
		fieldSample._value += MetaballFunction(pos, settings._center);

		if(color == true)
		{
			float dist = length(pos - settings._center);
			float factor = 10.0/dist;

			fieldSample._color += (settings._color * factor);
		}
	}

	if(color == true)
		fieldSample._color = normalize(fieldSample._color);
	
	return fieldSample;
}

//---------------------------------------------------------------------------
/// Calculates a normal vector for the given position the the metaball field.
//---------------------------------------------------------------------------
vec3 MetaballBaseNormal(vec3 pos, float value)
{
	// partial derivatives
	// Take discrete approximation by sampling function
	// Compute gradient

	float d = -0.1;
	float ff = value;
	float fx = MetaballField(vec3(pos.x + d, pos.y, pos.z), false)._value;
	float fy = MetaballField(vec3(pos.x, pos.y + d, pos.z), false)._value;
	float fz = MetaballField(vec3(pos.x, pos.y, pos.z + d), false)._value;

	vec3 normal = normalize(vec3(fx - ff, fy - ff, fz - ff));

	return normal;
}

//---------------------------------------------------------------------------
/// Samples the world space for metaballs.
//---------------------------------------------------------------------------
SampleGlobalResult SampleMetaBallMode(vec3 pos)
{
	SampleGlobalResult res;
	res._inside = false;

	MetaballFieldSample fieldSample = MetaballField(pos, true);
	
	if(fieldSample._value >= 20.2)
	{
		vec3 normal = normalize(MetaballBaseNormal(pos, fieldSample._value));

		res._inside = true;
		res._normal = normal;
		res._hasUVs = false;
		res._pos = pos;
		res._color = fieldSample._color;
	}
	
	return res;
}

// ----------------------------------------------------------------------
/// Samples the space and returns the result.
/// @param[in]	worldpos	Sample point in world space position as vec3.
/// @return					A SamppleResult object. If the sample point is not "inside", res._inside is false and all other values may be undefined.
// ----------------------------------------------------------------------
SampleGlobalResult SampleGlobalSpace(vec3 worldPos)
{
	if(u_objectMode == MODE_SPHERE)
	{
		return SampleSphereMode(worldPos);
	}

	if(u_objectMode == MODE_METABALL)
	{
		return SampleMetaBallMode(worldPos);
	}

	if(u_objectMode == MODE_DEFORMED_SPHERE)
	{

	}

	SampleGlobalResult res;
	res._inside = false;
	res._error = ERROR_ILLEGALMODE;
	return res;
}


// ----------------------------------------------------------------------
/// Samples the space until a surface was found.
// ----------------------------------------------------------------------
SampleGlobalResult SampleToSurface(vec3 startPos, vec3 sampleStep, int count)
{
	vec3 bigStep = sampleStep * 10.0;
	int bigCount = int(float(count) * .1);

	vec3 currentPos = startPos;

	SampleGlobalResult res;

	for(int i = 0; i < bigCount; ++i)
	{
		res = SampleGlobalSpace(currentPos);
		if(res._inside)
		{
			break;
		}

		currentPos = currentPos + bigStep;
	}

	if(res._inside == false)
		return res;

	// refine in the other direction

	vec3 reverseDirection = -sampleStep;

	SampleGlobalResult lastResut = res;
	

	currentPos = currentPos + reverseDirection;

	for(int i = 0; i < count; ++i)
	{
		res = SampleGlobalSpace(currentPos);
		if(res._inside)
		{
			lastResut = res;
		}
		else
		{
			break;
		}

		currentPos = currentPos + reverseDirection;
	}

	return lastResut;
}




// ----------------------------------------------------------------------
/// Shading utlity
// ----------------------------------------------------------------------



float LambertLighting(vec3 normal, vec3 lightDir)
{
	float diffuse = dot(lightDir, normal);
	diffuse = max(diffuse, 0);
	return diffuse;
}

float BlinnSpecular(vec3 normal, vec3 lightDir, vec3 pos)
{
	vec3 N = normal;
	vec3 L = lightDir;
		
	float diffuse = dot(L, N);
	diffuse = max(diffuse, 0);
	
	vec3 R = normalize(-reflect(L,N));
	vec3 E = normalize(camPos - pos);
	float specular = pow(max(dot(R,E),0.0),45);
	specular = max(specular, 0);
	
	float result = specular;
	return result;
}

float FresnelFx(vec3 normal, vec3 pos)
{
	float fresnel = dot(normal, normalize(camPos - pos));
	fresnel = 1.0 - fresnel;
	return fresnel;
}

bool HardShadow(vec3 pos)
{
	vec3 sampleDirection = GetLightDir();
	float scale = 0.05;
	vec3 sampleStep = sampleDirection * scale; 
	pos = pos + sampleStep;

	int steps = int((2.5 - pos.y)  / scale);

	SampleGlobalResult res = SampleToSurface(pos, sampleStep, 500);

	if(res._inside)
		return true;

	return false;
}

vec3 VolumeLight(vec3 pos)
{
	// sample out

	vec3 sampleDir = normalize(pos - camPos) * 0.02;
	
	vec3 currentPos = pos + sampleDir;

	float count = 0;

	vec3 lastPosInside = pos;

	for(int i = 0; i < 50; ++i)
	{
		SampleGlobalResult res = SampleGlobalSpace(currentPos);

		if(res._inside)
		{
			count++;
			lastPosInside = res._pos;
		}

		if(res._inside == false)
			break;

		currentPos = currentPos + sampleDir;
	}

	// find center

	vec3 center = (lastPosInside + pos) * .5;

	count = count * .5;

	// up
	vec3 sampleDirLight = GetLightDir() * 0.02;

	for(int i = 0; i < 100; ++i)
	{
		SampleGlobalResult res = SampleGlobalSpace(currentPos);

		if(res._inside)
		{
			count++;
		}

		currentPos = currentPos + sampleDirLight;
	}

	float value = count * 0.01;

	float red = clamp(1.0 - (value * 1.0), 0.0, 1.0);
	float green = clamp(1.0 - (value * 4.0), 0.0, 1.0);
	float blue = clamp(1.0 - (value * 9.0), 0.0, 1.0);	

	return vec3(red, green, blue);

}

vec3 FinalCompositing(SampleGlobalResult res)
{
	vec3 errorColor = vec3(1.0,0.0,1.0);

	if(res._inside == false)
		return errorColor;

	vec3 normal = res._normal;
	vec3 lightDir = GetLightDir();
	vec3 pos = res._pos;


	if(u_shadingMode == 1)
	{
		return normal;
	}

	if(u_shadingMode == 2)
	{
		return vec3(LambertLighting(normal, lightDir));
	}

	if(u_shadingMode == 3)
	{
		return vec3(BlinnSpecular(normal, lightDir,pos));
	}

	if(u_shadingMode == 4)
	{
		return vec3(FresnelFx(normal,pos));
	}

	if(u_shadingMode == 5)
	{
		float value = 1.0;
		if(HardShadow(pos))
			value = 0.0;

		return vec3(value);
	}

	if(u_shadingMode == 6)
	{
		return VolumeLight(pos);
	}

	if(u_shadingMode == 7)
	{
		return res._color;
	}

	if(u_shadingMode == 8)
	{
		if(res._hasUVs == false)
			return vec3(0.0,1.0,0.0);
		
		return vec3(res._uvs, 0.0);
	}

	if(u_shadingMode == 9)
	{
		vec3 baseColor = vec3(0.5,0.0,0.0);
		vec3 color = baseColor;
		float light = LambertLighting(normal, lightDir);
		float specular = BlinnSpecular(normal, lightDir,pos);
		float fresnel = FresnelFx(normal,pos);
		vec3 volumeLight = VolumeLight(pos);


		float shadow = 1.0;
		if(HardShadow(pos))
			shadow = 0.0;

		color = color *  light * shadow + (specular * shadow) + (volumeLight * 0.3);
		color += (fresnel* 0.6 *baseColor);
		color += (res._color * 0.1);
		return color;
	}

	if(u_shadingMode == 0)
	{
		vec3 color = res._color;
		float light = LambertLighting(normal, lightDir);
		float specular = BlinnSpecular(normal, lightDir,pos);
		float fresnel = FresnelFx(normal,pos);

		float shadow = 1.0;
		if(HardShadow(pos))
			shadow = 0.0;

		color = color *  light * shadow + (specular * shadow);
		color += (fresnel* 0.6 * res._color);
		color += (res._color * 0.1);
		return color;
	}

	// error
	return errorColor;
}




// ----------------------------------------------------------------------
/// Returns true if the given result object contains an error.
// ----------------------------------------------------------------------
bool HasError(SampleGlobalResult res)
{
	if(res._error != ERROR_NONE)
		return true;

	return false;
}

// ----------------------------------------------------------------------
/// Returns a color value based on the error value in the given result object.
/// Used for debugging.
// ----------------------------------------------------------------------
vec3 ErrorToColor(SampleGlobalResult res)
{
	if(res._error == ERROR_UNKNOWN)
		return vec3(1.0,0.0,0.0); // red

	if(res._error == ERROR_ILLEGALMODE)
		return vec3(1.0,1.0,0.0); // yellow

	return vec3(1.0,0.0,1.0);	// pink
}

#version 410

layout(location = 0) out vec4 vFragColor;	//fragment shader output

smooth in vec4 WorldSpacePos;

uniform vec3 camPos;

uniform float time;

uniform sampler2D	noiseTexture;


uniform int sphereMode;

uniform int renderMode;







// object mode
// 0: spheres
// 1: metaballs
// 2: defomred spheres
uniform int u_objectMode;

uniform float u_animation;

uniform int u_shadingMode;

// user controlled object
uniform float u_dynX;
uniform float u_dynY;


const vec3 sphereAPos = vec3(0,0,-1);
const float sphereARadius = 0.5;
const vec3 sphereBPos = vec3(-.5,-0.1,-1);
const float sphereBRadius = 0.3;
const vec3 sphereCPos = vec3(-.5,0.8,-1);
const float sphereCRadius = 0.2;

const int _sphereCount = 3;

float SampleNoise3D(vec3 worldPos)
{	
	float timeFactor = time * 6.0;

	float timeScaled = (sin(timeFactor) + 1.0) * 0.1;
	float timeScaledCos = (cos(timeFactor) + 1.0) * 0.1;

	float z = (worldPos.z + 2.0) * 0.5;
	float y = (worldPos.y + 1.0) * 0.5;
	float x = (worldPos.x + 2.0) * 0.25;
	
	z = (z + timeScaledCos) *.5;
	y = (y + timeScaled)*.5;
	x = (x + timeScaledCos)*.5;
	
	vec4 xy = texture( noiseTexture, vec2(x,y));
	vec4 zy = texture( noiseTexture, vec2(z,y));
	vec4 zx = texture( noiseTexture, vec2(z,x));
	
	float res = (xy.x + zy.x + zx.x) * 0.3333;
	
	return res;
}

bool Sample3DSine(vec3 worldPos)
{
	if(worldPos.y < 0.15)
		return false;
	
	if(worldPos.y > 0.65)
		return false;
		
	
	float x = abs((worldPos.x + 1.0)  *.5) * 6.28;
	
	
	float sinValue = (sin(x)  + 1.0) * .5;
	
	//float y = worldPos.y;
	//float sinValueY = (sin(y)  + 1.0) * .5;
	
	//float z = (worldPos.z + 2.0) * .5;
	//float sinValueZ = (sin(z)  + 1.0) * .5;
	
	if(sinValue > 0.5)
		return true;
	
	return false;
}

bool SampleSphere(vec3 worldPos, vec3 sphere, float radius)
{
	vec3 center = sphere;
	vec3 dist = worldPos - center;
	
	float distance = length(dist);
	
	if(sphereMode == 1)
	{
		float extra = SampleNoise3D(worldPos) / (distance*4.0);
		radius = radius + extra;
	}
	
	// in the core
	if(distance < radius)
	{
		return true;
	}
	
	return false;
}

vec3 GetSpherePos(int index)
{
	if(index == 0)
		return sphereAPos + vec3((sin(time) * 2.0), 0,0);
	
	if(index == 1)
		return sphereBPos + vec3((cos(time) * 1.0), 0,0);
	
	if(index == 2)
		return sphereCPos + vec3((cos(time + 3.14) * 2.0), 0,0);
	
	return vec3(0);
}

float GetSphereRadius(int index)
{
	if(index == 0)
		return sphereARadius;
	
	if(index == 1)
		return sphereBRadius + (sin(time) * 0.2);
	
	if(index == 2)
		return sphereCRadius + (sin(time + 3.14) * 0.1);
	
	return 0.0;
}

bool IsSphere(vec3 worldPos, int index)
{	
	return SampleSphere(worldPos, GetSpherePos(index), GetSphereRadius(index));
}


bool SampleSpace(vec3 worldPos)
{		
	for(int i = 0; i < _sphereCount; ++i)
	{
		if(IsSphere(worldPos, i))
			return true;
	}

	return false;

	// TODO: use prime numbers to avoid periodic animation

	float effect = sin(time);
	float effect2 = cos(time * 2.5f);
	float effect3 = sin(time* 0.3 + 100.0f);
	float fxAbs = (effect2 *.25) + .5;
	float fxAbs2 = (sin(time*3.3) *.25) + .5;
	float fxAbs3 = (effect3 *.25) + .5;
	
	//if(Sample3DSine(worldPos))
	//	return vec4(value, value, value, 1.0);
	
	//return result;
	
	
	// grundkugel
	if(SampleSphere(worldPos, vec3(effect3*.8,-0.2 + fxAbs,-1), 0.5*fxAbs3))
		return true;

	if(SampleSphere(worldPos, vec3(-0.8 * effect,.5 * fxAbs3,-1), 0.6 * fxAbs2))
		return true;
	
	if(SampleSphere(worldPos, vec3(-0.8 * effect3,.5 * fxAbs2,-1), 0.5 * fxAbs))
		return true;
	
	if(SampleSphere(worldPos, vec3(effect2 * effect,0.5*fxAbs,-1.0), 0.3 * fxAbs2))
		return true;
	
	if(SampleSphere(worldPos, vec3(0.9 * effect3,0.5*fxAbs3,-1), 0.3 * fxAbs))
		return true;
	
	
	if(SampleSphere(worldPos, vec3(2.0 * effect,0.8*fxAbs,-1), 0.05 * fxAbs3))
		return true;
	

	return false;
}

vec3 GetSphereNormal(vec3 worldPos)
{	
	for(int i = 0; i < _sphereCount; ++i)
	{
		if(IsSphere(worldPos, i))
			return normalize(worldPos - GetSpherePos(i));	
	}
	
	return vec3(0);
}

vec3 GetLightDir()
{
	return normalize(vec3(-0.2,1,1.0));
}

struct SphereCenterNormal
{
	bool _inside;
	vec3 _center;
	vec3 _normal;
};

SphereCenterNormal GetSphereCenterNormal(vec3 worldPos)
{
	SphereCenterNormal res;
	res._inside = false;
	
	for(int i = 0; i < _sphereCount; ++i)
	{
		 float radius = GetSphereRadius(i);
		 vec3 spherePos = GetSpherePos(i);
		 
		 if(SampleSphere(worldPos, spherePos, radius))
		 {
			 res._inside = true;
			 res._center = spherePos;
			 res._normal = normalize(worldPos - spherePos);
			 return res;
		 }
	}
	
	return res;
}

struct SampleResult
{
	bool _inside;
	vec3 _normal;
	float _u;
	float _v;
};

SampleResult GetSampleResult(vec3 worldPos)
{
	SampleResult res;
	res._inside = false;
	
	SphereCenterNormal scn = GetSphereCenterNormal(worldPos);
	if(scn._inside)
	{
		res._inside = true;
		res._normal = scn._normal;
		
		// https://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere
		vec3 d = - normalize(res._normal);
		float u = 0.5 + atan(d.z, d.x) / 6.2831;
		float v = 0.5 - asin(d.y) / 3.1415;
		
		res._u = u;
		res._v = v;
	}
	
	return res;
}



// ----------------------------------------------------------------------
// NEW WORLD
// ----------------------------------------------------------------------



const int MODE_SPHERE = 1;
const int MODE_METABALL = 2;
const int MODE_DEFORMED_SPHERE = 3;


// error codes for SampleResult::_error
const int ERROR_NONE = 0;
const int ERROR_UNKNOWN = 1;
const int ERROR_ILLEGALMODE = 2;


const float CONST_PI = 3.1415;
const float CONST_TAU = 6.2831;

// ----------------------------------------------------------------------
/// Structure storing data from sampling space with SampleSpace().
// ----------------------------------------------------------------------
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


// ----------------------------------------------------------------------
/// Animation Utlity
// ----------------------------------------------------------------------

float GetAnimation01(float timeFactor)
{
	return (sin(u_animation * timeFactor) + 1.0) * .5;
}

float GetAnimation11(float timeFactor)
{
	return sin(u_animation * timeFactor);
}

vec3 GetDynamicObject()
{
	return vec3(u_dynX, u_dynY, -1.0);
}



// ----------------------------------------------------------------------
/// SPHERE MODE
// ----------------------------------------------------------------------

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

MetaballSettings GetMetaballSettings(int i)
{
	MetaballSettings settings;

	if(i == 0)
	{
		float period = GetAnimation11(1.0);
		vec3 pos1 = vec3(-1*period,0.6,- 1.0);
		settings._center = pos1;
		settings._color = vec3(1.0,0.0,0.0);
	}

	if(i == 1)
	{
		float period = GetAnimation11(1.0);
		vec3 pos2 = vec3(1*period,0.3, -1.0);
		settings._center = pos2;
		settings._color = vec3(0.0,1.0,0.0);
	}

	if(i == 2)
	{
		settings._center = GetDynamicObject();
		settings._color = vec3(0.0,0.0,1.0);
	}

	return settings;
}

float MetaballBaseFunction(vec3 pos, vec3 center)
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

MetaballFieldSample MetaballBaseField(vec3 pos, bool color)
{
	MetaballFieldSample fieldSample;
	fieldSample._value = 0.0;
	fieldSample._color = vec3(0.0);

	for(int i = 0; i < 3; ++i)
	{
		MetaballSettings settings = GetMetaballSettings(i);
		fieldSample._value += MetaballBaseFunction(pos, settings._center);

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


vec3 MetaballBaseNormal(vec3 pos, float value)
{
	// partial derivatives
	// Take discrete approximation by sampling function
	// Compute gradient

	float d = -0.1;
	float ff = value;
	float fx = MetaballBaseField(vec3(pos.x + d, pos.y, pos.z), false)._value;
	float fy = MetaballBaseField(vec3(pos.x, pos.y + d, pos.z), false)._value;
	float fz = MetaballBaseField(vec3(pos.x, pos.y, pos.z + d), false)._value;

	vec3 normal = normalize(vec3(fx - ff, fy - ff, fz - ff));

	return normal;
}

SampleGlobalResult SampleMetaBallMode(vec3 pos)
{
	SampleGlobalResult res;
	res._inside = false;

	MetaballFieldSample fieldSample = MetaballBaseField(pos, true);
	
	if(fieldSample._value >= 7.2)
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
/// @param[in]	worldpos		Sample point in world space position as vec3.
/// @return						A SamppleResult object. If the sample point is not "inside", res._inside is false and all other values may be undefined.
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
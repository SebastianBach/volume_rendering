
// see "OpenGL Development Cookbook"
// Chapter "Implementing pseudo-isosurface rendering in single-pass GPU ray casting"

const float DELTA = 0.01;	

float GetDensity(vec3 worldPos)
{
	bool result = SampleSpace(worldPos);
	
	if(result)
		return 0.1;
	
	return 0.0;
}

vec3 GetGradient(vec3 worldPos) 
{
    vec3 s1, s2;  
	
    //Using center finite difference 
    s1.x = GetDensity(worldPos-vec3(DELTA,0.0,0.0));
    s2.x = GetDensity(worldPos+vec3(DELTA,0.0,0.0));

    s1.y = GetDensity(worldPos-vec3(0.0,DELTA,0.0));
    s2.y = GetDensity(worldPos+vec3(0.0,DELTA,0.0));

    s1.z = GetDensity(worldPos-vec3(0.0,0.0,DELTA));
    s2.z = GetDensity(worldPos+vec3(0.0,0.0,DELTA));

    return normalize((s1-s2)/2.0); 
}


vec4 BeautyRender(vec3 firstVolumePos)
{
	return vec4(1.0,0,0,1.0);
}

vec4 RenderNormal(vec3 firstVolumePos)
{
	return vec4(GetSphereNormal(firstVolumePos), 1.000);
}

vec4 RenderFresnelEffect(vec3 firstVolumePos)
{
	vec3 newNormal = GetSphereNormal(firstVolumePos);
	float fresnel = dot(newNormal, normalize(camPos - firstVolumePos));
	fresnel = 1.0 - fresnel;
			
	return vec4(fresnel, fresnel, fresnel, 1.000);
}

vec4 RenderLambertLighting(vec3 firstVolumePos)
{
	vec3 normal = GetSphereNormal(firstVolumePos);
	float diffuse = dot(GetLightDir(), normal);
	diffuse = max(diffuse, 0);
	
	return vec4(diffuse, diffuse, diffuse, 1.000);
}

vec4 RenderLambertPhongLighting(vec3 firstVolumePos)
{
	vec3 N = GetSphereNormal(firstVolumePos);
	vec3 L = GetLightDir();
		
	float diffuse = dot(L, N);
	diffuse = max(diffuse, 0);
	
	vec3 R = normalize(-reflect(L,N));
	vec3 E = normalize(camPos - firstVolumePos);
	float specular = pow(max(dot(R,E),0.0),45);
	specular = max(specular, 0);
	
	float result = specular;
	
	return vec4(result, result, result, 1.000);
}

vec4 RenderHardShadow(vec3 firstVolumePos)
{
	float result = 1.0;
	
	vec3 shadowStep = GetLightDir() * 0.05;
	vec3 currentPos = firstVolumePos + shadowStep;
	
	for(int i = 0; i < 100; ++i)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			result = 0.0;
			break;
		}
		
		currentPos = currentPos + shadowStep;
	}
	
	return vec4(result, result, result, 1.000);
}

vec4 RenderSoftVolumeShadow(vec3 firstVolumePos)
{
	float result = 1.0;
	
	vec3 shadowStep = GetLightDir() * 0.01;
	vec3 currentPos = firstVolumePos + shadowStep;
	
	for(int i = 0; i < 100; ++i)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			result = result - 0.02;
		}
		
		if(result <= 0.0)
		{
			result = 0.0;
			break;
		}
		
		currentPos = currentPos + shadowStep;
	}
	
	
	return vec4(result, result, result, 1.000);
}

vec4 HalfVolumeLightModel(vec3 firstVolumePos)
{
	vec3 viewDir = normalize(firstVolumePos - camPos);
	
	float stepSize = 0.01;
	
	// get depth
	vec3 currentPos = firstVolumePos;
	
	int steps = 100;
	vec3 step = viewDir * stepSize;
	
	vec3 lastVolumePos;
	int occlusionDepth = 0;
		
	for(int i=0;i<steps;i++)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			lastVolumePos = currentPos;
			occlusionDepth = occlusionDepth + 1;
		}
		
		currentPos = currentPos + step;
	}
	
	// sample to light
	vec3 centerPoint = (firstVolumePos + lastVolumePos) * .5;
	
	vec3 lightStep = GetLightDir() * stepSize;
	
	vec3 lightSamplePos = centerPoint + lightStep;
	
	int occlusionLight = 0;
	
	for(int i=0;i<steps;i++)
	{
		bool insideVolume = SampleSpace(lightSamplePos);
		
		if(insideVolume)
		{
			occlusionLight = occlusionLight + 1;
		}
		
		lightSamplePos = lightSamplePos + lightStep;
	}
	
	float value = 100.0;
	
	// absorb into volume 	
	value = (float(occlusionLight) * 0.001) * value;
	// diffuse in center
	//value = value * 0.1;
	// absorb in depth
	value = (float(occlusionDepth) * 0.001) * value;
	
	occlusionDepth = occlusionDepth / 2;
	float occDepthF = float(occlusionDepth);
	
	int fullOcclusion = occlusionLight + occlusionDepth;
	float fOcclusion = float(fullOcclusion) * 0.03;
	
	if(value > 1.0)
		return vec4(1.0, 0, 0, 1.000);
	
	value = clamp(value, 0,1);
	
	float red = 1.0 -(fOcclusion * 0.2);
	float green = 1.0 -(fOcclusion * 0.4);
	float blue = 1.0 -(fOcclusion * 0.6);
	
	red = clamp(red, 0,1);
	green = clamp(green, 0,1);
	blue = clamp(blue, 0,1);
	
	// only occDepth defines how dense the fog in the line of sight is
	return vec4(red, green, blue, 1.0);
}

bool CheckerboardShader(float u, float v)
{
	float offset = time;
	
	float uv = mod(u + offset,0.1);
	float vv = mod(v + offset, 0.1);
	
	bool resu = uv >= 0.05;
	bool resv = vv >= 0.05;
	
	if(resu != resv)
		return true;
	
	return false;
}

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

float MetaballField(vec3 pos)
{
	float period = (sin(time) + 1.0) * .5;
	float period2 = (sin(time*2) + 1.0) * .5;
	
	vec3 pos1 = vec3(-1*period,0.6,- 1.0);
	vec3 pos2 = vec3(1*period,0.3, -1.0);
	vec3 pos3 = vec3(-4 * period + 1.0, 0.45, -1.0);
	
	float res = MetaballFunction(pos, pos1);
	res += MetaballFunction(pos, pos2);
	res += MetaballFunction(pos, pos3);
	
	return res;
}

// see http://blackpawn.com/texts/metanormals/default.html
vec3 MetaballNormal(vec3 pos)
{
	// vec3 normal;
	
	
	// partial derivatives
	// Take discrete approximation by sampling function
	// Compute gradient

	float d = -0.1;
	float ff = MetaballField(pos);
	float fx = MetaballField(vec3(pos.x + d, pos.y, pos.z));
	float fy = MetaballField(vec3(pos.x, pos.y + d, pos.z));
	float fz = MetaballField(vec3(pos.x, pos.y, pos.z + d));

	vec3 normal = normalize(vec3(fx - ff, fy - ff, fz - ff));
	//normal.y = 0;
	//normal.z = 0;
	return normal;

	//float f = pow(pos.x - center.x, 2) + pow(pos.y - center.y, 2) +  pow(pos.z - center.z, 2);
	//f = pow(f, 2);
	//f = 1 / f;

	//vec3 base = vec3(2*(center.x - pos.x), 2* (center.y - pos.y), 2* (center.z - pos.z));

	//base = f * base;
	

	//return base;
}

struct MetaballResult
{
	bool _inside;
	float _value;
	vec3 _normal;
};




MetaballResult CheckMetaBalls(vec3 pos)
{

	MetaballResult metaRes;
	metaRes._inside = false;
	

	//float period = (sin(time) + 1.0) * .5;
	
	//vec3 pos1 = vec3(-1*period,0.6,- 1.0);
	//vec3 pos2 = vec3(1*period,0.3, -1.0);
	
	
	//float res = MetaballFunction(pos, pos1);
	//res += MetaballFunction(pos, pos2);
	
	float res = MetaballField(pos);

	vec3 normal = normalize(MetaballNormal(pos));
	
	float threshold = 0.10 + (0.05 * (sin(time) + 1.0) * .5);
	// 34
	if(res >= 7.2)
	{
		metaRes._inside = true;
		metaRes._normal = normal;
	}
	
	return metaRes;
}

bool MetaBallInside(vec3 pos, vec3 center)
{
	float factor = MetaballFunction(pos, center);
	
	if(factor <= 0.08)
		return true;
	
	return false;
}



void main()
{

	// NEW WORLD
	vec4 newResult;


	vec3 startPos = WorldSpacePos.xyz;
	vec3 sampleDirection = normalize(WorldSpacePos.xyz - camPos);
	vec3 sampleStep = sampleDirection * 0.002; 

	SampleGlobalResult res = SampleToSurface(startPos, sampleStep, 1000);

	if(HasError(res))
	{
		vFragColor = vec4(ErrorToColor(res), 1.0);
		return;
	}

	if(res._inside)
	{
		vec3 finalColor = FinalCompositing(res);
		newResult = vec4(finalColor, 1.0);
	}

	vFragColor = newResult;
	
	return;






	if(renderMode == 8)
	{
		// sample
		vec3 currentPos = WorldSpacePos.xyz;
		vec3 firstVolumePos;
		bool surfaceDetected = false;
		vec3 dir = normalize(WorldSpacePos.xyz - camPos);
		vec3 detectFirstVolumeStep = dir * 0.01;

		vec3 normal = vec3(1.0,0,0);
		
		for(int i = 0; i < 200; ++i)
		{
			MetaballResult res = CheckMetaBalls(currentPos);
			if(res._inside)
			{
				normal = res._normal;
				firstVolumePos = currentPos;
				surfaceDetected = true;
				break;
			}
			
			currentPos = currentPos + detectFirstVolumeStep;
		}
		
		vec4 renderResult;
		
		if(surfaceDetected)
		{
			float diffuse = dot(GetLightDir(), normal);
			diffuse = max(diffuse, 0);

			renderResult = vec4(vec3(diffuse),1.0);
		}
		else
		{
			renderResult = vec4(0.0,0.0,0.5,1.0);
		}
		
		vFragColor = renderResult;
		
		return;
	}
	
	
	//vec2 coord = (WorldSpacePos.xy + vec2(1.0)) * 0.5;
	//vec4 texColor = texture( noiseTexture, coord );
	
	//float resultNoise = SampleNoise3D(WorldSpacePos.xyz);
	//vFragColor = vec4(vec2(resultNoise), 1.0, 1.0);
	//return;
	
	int steps = 50;
	float stepSize = 0.02;
	
	vec3 dir = normalize(WorldSpacePos.xyz - camPos);
	
	vec3 step = dir * stepSize;
	
	vec3 currentPos = WorldSpacePos.xyz;
	
	vec4 result = vec4(0,0,0,0.0);
	
	vec3 lightDirStep = GetLightDir() * stepSize;
	
	float lightValue = 0.01;
	
	float occlusion = 0;
	
	bool surfaceDetected = false;
	
	bool surfaceVolumeFound = false;
	bool surfaceVolumeInShadow = false;
	
	vec3 lastVolumePos;
	
	
	
	
	// get first rough position of surface volume
	float firstDetectionStepSize = 0.1;
	vec3 detectFirstVolumeStep = dir * 0.1;
	vec3 firstVolumeStartPos = WorldSpacePos.xyz;
	vec3 firstVolumePos;
	
	vec3 startPoint = WorldSpacePos.xyz;
	vec3 endPoint = WorldSpacePos.xyz + (dir); 
	
	for(int i = 0; i < 50; ++i)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			firstVolumePos = currentPos;
			surfaceDetected = true;
			break;
		}
		
		currentPos = currentPos + detectFirstVolumeStep;
	}
	
	// if no surface found just return
	if(surfaceDetected == false)
	{
		vFragColor = vec4(0.0);
		return;
	}
	
	// refine surface position
	currentPos = firstVolumePos;
	float fineDetectionStepSize = firstDetectionStepSize / 50;
	detectFirstVolumeStep = -dir * fineDetectionStepSize;
	// old 0.002
	
	for(int i = 0; i < 50; ++i)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			firstVolumePos = currentPos;
		}
		
		if(insideVolume == false)
			break;
		
		currentPos = currentPos + detectFirstVolumeStep;
	}
	
	vec4 renderResult;
	
	// rendering
	if(renderMode == 0)
	{
		
		// renderResult = BeautyRender(firstVolumePos);
		
		SampleResult res = GetSampleResult(firstVolumePos);
		
		
		vec4 noise = texture( noiseTexture, vec2(res._u,res._v));
		
		bool inside = CheckerboardShader(res._u, res._v);
		
		//renderResult = vec4(noise.xyz, 1.0);
		
		if(inside)
			renderResult = vec4(1.0,1.0,1.0, 1.0);
		else
			renderResult = vec4(0.0,0.0,0.0,1.0);
		
			
	}
	
	if(renderMode == 1)
	{
		
		SampleResult res = GetSampleResult(firstVolumePos);
		
		renderResult = vec4(res._normal, 1.0);
	}
	
	if(renderMode == 2)
	{
		// render fresnel effect
		renderResult = RenderFresnelEffect(firstVolumePos);
	}
	
	if(renderMode == 3)
	{
		renderResult = RenderLambertLighting(firstVolumePos);
	}
	
	if(renderMode == 4)
	{
		renderResult = RenderLambertPhongLighting(firstVolumePos);
	}
	
	if(renderMode == 5)
	{
		renderResult = RenderHardShadow(firstVolumePos);
	}
	
	if(renderMode == 6)
	{
		renderResult = RenderSoftVolumeShadow(firstVolumePos);
	}
	
	if(renderMode == 7)
	{
		renderResult = HalfVolumeLightModel(firstVolumePos);
	}
	
	if(renderMode == 9)
	{
		vec4 halfVolume = HalfVolumeLightModel(firstVolumePos);
		
		vec4 lambert = RenderLambertLighting(firstVolumePos);
		vec4 softShadow = RenderSoftVolumeShadow(firstVolumePos);
		
		vec4 spec = RenderLambertPhongLighting(firstVolumePos);
		vec4 fresnel = RenderFresnelEffect(firstVolumePos);
		
		vec4 hardShadow = RenderHardShadow(firstVolumePos);
		
		vec4 localLight = (lambert) * softShadow  * .5 + (spec * hardShadow) + (fresnel * 0.3);
		renderResult = localLight;
	
		renderResult.a = 1.0;
	}
	
	vFragColor = renderResult;
	return;
	
	
	// search for volume thickness
	if(surfaceDetected)
	{
		currentPos = firstVolumePos;
		
		for(int i=0;i<steps;i++)
		{
			bool insideVolume = SampleSpace(currentPos);
			if(insideVolume)
			{
				lastVolumePos = currentPos;
				occlusion = occlusion + 1.0;
			}
			
			currentPos = currentPos + step;
		}
	}
	
	// search for shadow
	surfaceVolumeInShadow = false;
	float shadowValue = 0.0;
	
	vec3 shadowStep = GetLightDir() * 0.05;
	currentPos = firstVolumePos + shadowStep;
	
	for(int i = 0; i < 100; ++i)
	{
		bool insideVolume = SampleSpace(currentPos);
		if(insideVolume)
		{
			surfaceVolumeInShadow = true;
			shadowValue = shadowValue + 0.05;
		}
		
		if(shadowValue >= 1.0)
			break;
		
		currentPos = currentPos + shadowStep;
	}
	
	if(surfaceDetected)
	{
		
		float light = 1.0;
		
		
		if(surfaceVolumeInShadow)
		{
			light = 0.0;
			result =  vec4(light, light, light, 1.000);
		}
		else
		{
			vec3 normal = GetSphereNormal(firstVolumePos);
			//result =  vec4(normal, 1.000);
			
			float f = dot(GetLightDir(), normal);
			
			float fresnel = dot(normal, normalize(camPos - firstVolumePos));
			f = fresnel;
			
			result =  vec4(f, f, f, 1.000);
			
			
		}
		
		vec3 newNormal = GetSphereNormal(firstVolumePos);
		float fresnel = dot(newNormal, normalize(camPos - firstVolumePos));
		fresnel = 1.0 - fresnel;
			
		result =  vec4(fresnel, fresnel, fresnel, 1.000);
		
		
		
		vFragColor = result;
		return;
	}
	
	if(surfaceDetected)
	{
		
		
		// shading
		float subOcclusion = 0.0;
		
		// center point
		vec3 centerPoint = (firstVolumePos + lastVolumePos) * .5;
		
		
		vec3 lightSamplePos = centerPoint + lightDirStep;
		
		// get samples from light
		// optimierung: "steps" kann abhängig gemacht werden von der y-position;
		// punkte ganz oben müssen nicht mehr so oft in richtung der lichtquelle sampeln
		for(int j = 0; j < steps; j++)
		{
			bool insideVolume = SampleSpace(lightSamplePos);
			
			if(insideVolume)
			{
				subOcclusion = subOcclusion + 1.0;
			}
			
			lightSamplePos = lightSamplePos + lightDirStep;
		}
		
		if(surfaceVolumeFound == false)
		{
			// this is the surface volume element
			surfaceVolumeFound = true;
		}
		
		// in center mode half the "occlusion)
		occlusion = occlusion +.5f;
		
		float resOcclusion = occlusion + subOcclusion;
		float lightValue = 1.0 - (resOcclusion * 0.02);
		
		float red = 0.7 - (resOcclusion * 0.005);
		float green = 0.7 - (resOcclusion * 0.01);
		float blue = 0.7 - (resOcclusion * 0.015);
		
		if(red < 0.0)
			red = 0.0;
		
		if(green < 0.0)
			green = 0.0;
		
		if(blue < 0.0)
			blue = 0.0;
		
		result =  vec4(red, green, blue, 0.000);
		
		
		
		// shading
		
		// IDAEA: add fresnel effect
		// IDEA: check for shadow of firstVolumePos to check for shadow
		
		/*
		vec3 normal = GetGradient(firstVolumePos);
		
		// simple fresnel effect
		
		vec3 toCam = camPos - firstVolumePos;
		
		float angle =  dot( toCam, -normal );
		float shade = cos(angle * 2.0);
		*/
		
		//result.x = shade;
		//result.y = shade;
		//result.z = shade;
		
		//result = vec4(normal,1.0);
		
		//result = vec4(1.0);
		/*
		float angleLight = dot(normal, GetLightDir());
		if(angleLight > 0.0 && surfaceVolumeInShadow == false)
		{
			float specular = pow(max(0.0, dot(reflect(-GetLightDir(), normal), toCam)),
	      5.0);
		  
			result.xyz = result.xyz + vec3(specular * .001);
		}
		*/
		
		// just add light when there is not shadow
		// done to not hide the SSS effect
		{
			float lightValue = 0.4 * (1.0 - shadowValue);
			result = vec4(result.xyz + vec3(lightValue, 0.0, 0.0), 1.0);
		}
		
		result.a = 1.0;
	}
	
	
	
	vFragColor = result;
}
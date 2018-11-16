void main()
{
	vec3 _startPos = WorldSpacePos.xyz;
	vec3 sampleDirection = GetLightDir();
	vec3 sampleStep = sampleDirection * 0.05; 

	SampleGlobalResult res = SampleToSurface(_startPos, sampleStep, 40);

	vec4 newResult;
	vec4 _base = vec4(0.5,0.5,0.5,1.0);
	vec4 _shadowColor = vec4(0.4,0.4,0.4,1.0);

	if(res._inside)
	{
		newResult = vec4(res._color, 1.0);
	}
	else
	{
		newResult = _base;
	}

	vFragColor = newResult;
	
	return;






	// int steps = 100;
	// float stepSize = 0.05;
	
	
	// vec3 lightDirStep = GetLightDir() * stepSize;
	
	// bool shadow = false;
	
	// vec3 currentPos = WorldSpacePos.xyz + lightDirStep;
	
	// for(int i = 0; i < steps; ++i)
	// {
	// 	// sample
	// 	bool insideVolume = SampleSpace(currentPos);
	// 	if(insideVolume)
	// 	{
	// 		shadow = true;
	// 		break;
	// 	}
		
	// 	currentPos = currentPos + lightDirStep;
	// }
	
	// vec4 base = vec4(0.5,0.5,0.5,1.0);
	// vec4 shadowColor = vec4(0.4,0.4,0.4,1.0);
	
	// if(renderMode == 7)
	// {
	// 	base = vec4(0.9,0.5,0.3,1.0);
	// 	shadowColor = vec4(0.8,0.4,0.2,1.0);
	// // }
	
	// // if(renderMode == 9)
	// {
	// 	// base = vec4(0.1,0.1,0.1,1.0);
	// 	// shadowColor = vec4(0.0,0.0,0.0,1.0);
	// }
	
	// vec4 result = base;
	
	// if(shadow)
	// {
	// 	result = shadowColor;
	// }
	
	// vFragColor = result;
}

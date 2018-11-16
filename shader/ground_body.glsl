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
}

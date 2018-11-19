void main()
{
	vec3 startPos = WorldSpacePos.xyz;
	vec3 sampleDirection = vec3(0.0,1.0,0.0);
	vec3 sampleStep = sampleDirection * 0.005;

	startPos = startPos +  sampleStep;

	SampleGlobalResult res = SampleToSurface(startPos, sampleStep, 400);

	vec4 newResult;
	vec4 _base = vec4(0.5,0.5,0.5,1.0);
	vec4 _shadowColor = vec4(0.4,0.4,0.4,1.0);

	if(HasError(res))
	{
		vFragColor = vec4(ErrorToColor(res), 1.0);
		return;
	}

	if(res._inside)
	{
		vec3 finalColor = FinalCompositing(res);
		newResult = vec4(finalColor * .5, 1.0);
	}
	else
	{
		newResult = vec4(0,0,0,1.0);
	}

	vFragColor = newResult;
	
	return;
}

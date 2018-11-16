
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
}

void main()
{
	vec4 newResult;


	vec3 startPos = s_worldSpacePos.xyz;
	vec3 sampleDirection = normalize(s_worldSpacePos.xyz - u_camPos);
	vec3 sampleStep = sampleDirection * 0.01; 

	SampleGlobalResult res = SampleToSurface(startPos, sampleStep, 200);

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
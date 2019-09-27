#pragma once
class cCamera
{
private:
	vec3_t position;

public:
	void Update();
	void GetPositionInCamera(vec3_t res, vec3_t position);
};
#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"

struct NoiseVector {
	float x, y, z;
	NoiseVector() { x = 0; y = 0; z = 0; }
	NoiseVector(float pX, float pY, float pZ = 0.f) { x = pX; y = pY; z = pZ; }
	NoiseVector(int px, int py, int pz) { x = px; y = py; z = pz; }
	NoiseVector(NoiseVector p1, NoiseVector p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
		z = p2.z - p1.z;
	}
	NoiseVector(FVector fv) { x = fv.X; y = fv.Y; z = fv.Z; }

	float operator*(NoiseVector v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	void toUnitVector()
	{
		float length = sqrt(x * x + y * y + z * z);
		if (length == 0.f)
			return;
		x /= length;
		y /= length;
		z /= length;
	}
};

class MC_FAKE_API PerlinNoise
{
public:
	PerlinNoise();
	PerlinNoise(int Seed);
	float Noise(int x, int y, int z, int Gridsize, float Amplitude = 1.f);
	float OctaveNoise(int x, int y, int z, int Gridsize, float Amplitude, int Octaves);
	void SetSeed(int NewSeed);

private:
	FRandomStream RandStream;
	int32 seed;
	NoiseVector ToCornerPoint(int x, int y, int z, int Gridsize);
	int32 Hash(int x, int y, int z);
	NoiseVector RandUnitVector(int x, int y, int z = 0.f);
	float Interp2Values(float v1, float v2, float alpha);
	float Interp4Values(const TArray<float>& Values, NoiseVector InterpPoint, int GridSize);
	float Smooth(float v);

};

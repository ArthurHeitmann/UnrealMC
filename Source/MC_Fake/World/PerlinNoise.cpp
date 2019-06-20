#include "PerlinNoise.h"

PerlinNoise::PerlinNoise()
{

	seed = 43509;
}

PerlinNoise::PerlinNoise(int Seed)
{
	PerlinNoise();
	seed = Seed;
}

float PerlinNoise::Noise(int x, int y, int z, int Gridsize, float Amplitude)
{
	TArray<NoiseVector> CornerPoints = {
		{ToCornerPoint(x, y, z, Gridsize)},
		{ToCornerPoint(x + Gridsize, y, z, Gridsize)},
		{ToCornerPoint(x, y + Gridsize, z, Gridsize)},
		{ToCornerPoint(x + Gridsize, y + Gridsize, z, Gridsize)},
		{ToCornerPoint(x, y, z + Gridsize, Gridsize)},
		{ToCornerPoint(x + Gridsize, y, z + Gridsize, Gridsize)},
		{ToCornerPoint(x, y + Gridsize, z + Gridsize, Gridsize)},
		{ToCornerPoint(x + Gridsize, y + Gridsize, z + Gridsize, Gridsize)}
	};
	TArray<NoiseVector> GradientVectors;
	GradientVectors.SetNumUninitialized(8);
	for (int i = 0; i < 8; i++)
		GradientVectors[i] = RandUnitVector(CornerPoints[i].x, CornerPoints[i].y, CornerPoints[i].z);
	TArray<float> DotProducts;
	DotProducts.SetNumUninitialized(8);
	for (int i = 0; i < 8; i++)
	{
		NoiseVector DirectionVector(CornerPoints[i], {x, y, z});
		DirectionVector.x /= Gridsize;
		DirectionVector.y /= Gridsize;
		DirectionVector.z /= Gridsize;
		DotProducts[i] = GradientVectors[i] * DirectionVector;
	}
	float w;
	if (z >= 0)
		w = (float) (z - static_cast<int>(z / Gridsize) * Gridsize) / Gridsize;
	else
		w = (float) (z + static_cast<int>(abs(z) / Gridsize) * Gridsize) / Gridsize;
	w = Smooth(w);

	float TopInterp = Interp4Values(DotProducts, {x, y, z}, Gridsize);
	TArray<float> LastHalf;
	LastHalf.SetNumUninitialized(4);
	for (int i = 0; i < 4; i++)
		LastHalf[i] = DotProducts[i + 4];
	float BottomInterp = Interp4Values(LastHalf, {x, y, z}, Gridsize);
	return Interp2Values(BottomInterp, TopInterp, w) * Amplitude;
}

float PerlinNoise::OctaveNoise(int x, int y, int z, int Gridsize, float Amplitude, int Octaves)
{
	float total = 0;
	for (int i = 0; i < Octaves; i++)
		total += Noise(x, y, z, 1 / pow(2, i) * Gridsize, pow(0.5, i)) * Amplitude /** (1 / pow(2, i)) * Gridsize*/;
	return total;
}

void PerlinNoise::SetSeed(int NewSeed)
{
	seed = NewSeed;
}

NoiseVector PerlinNoise::ToCornerPoint(int x, int y, int z, int Gridsize)
{
	return {
		static_cast<int>(x / Gridsize) * Gridsize,
		static_cast<int>(y / Gridsize) * Gridsize,
		static_cast<int>(z / Gridsize) * Gridsize,
	};
}

int32 PerlinNoise::Hash(int x, int y, int z)
{
	/*int i = RandomNumbers[RandomNumbers[x] % RandomNumbers[y]] % RandomNumbers[z];
	UE_LOG(LogTemp, Warning, TEXT("Rand: %d"), i);
	return i;*/

	return (x + y) ^ (y + z) ^ (z + seed);
}

NoiseVector PerlinNoise::RandUnitVector(int x, int y, int z)
{
	RandStream.Initialize(Hash(x, y, z));
	return RandStream.VRand();
}

float PerlinNoise::Interp2Values(float v1, float v2, float alpha)
{
	return (v1 * (1.f - alpha) + (v2 * alpha));
}

float PerlinNoise::Interp4Values(const TArray<float>& Values, NoiseVector InterpPoint, int GridSize)
{
	float u, v;
	if (InterpPoint.x >= 0)
		u = (float) (InterpPoint.x - static_cast<int>(InterpPoint.x / GridSize) * GridSize) / GridSize;
	else
		u = (float) (InterpPoint.x + static_cast<int>(abs(InterpPoint.x) / GridSize + 1.f) * GridSize) / GridSize;
	if (InterpPoint.y >= 0)
		v = (float) (InterpPoint.y - static_cast<int>(InterpPoint.y / GridSize) * GridSize) / GridSize;
	else
		v = (float) (InterpPoint.y + static_cast<int>(abs(InterpPoint.y) / GridSize + 1.f) * GridSize) / GridSize;

	u = Smooth(u);
	v = Smooth(v);
	float bottomInterp = Interp2Values(Values[0], Values[1], u);
	float topInterp = Interp2Values(Values[2], Values[3], u);
	return Interp2Values(bottomInterp, topInterp, v);
}

float PerlinNoise::Smooth(float v)
{
	return v * v * v * (6.f * v * v - 15.f * v + 10.f);
}

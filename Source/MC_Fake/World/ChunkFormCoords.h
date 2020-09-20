#pragma once

#include "CoreMinimal.h"

#include "Misc/Stringable.h"

struct FChunkFormCoords2D : public IStringable {
public:
	int32 X, Y;

	FChunkFormCoords2D() {}
	FChunkFormCoords2D(int32 x, int32 y) : X(x), Y(y) {}
	FChunkFormCoords2D(float X, float Y) : X((int32) X), Y((int32) Y) {}
	
	bool operator==(const FChunkFormCoords2D& c) const {
		return X == c.X && Y == c.Y;
	}
	bool operator!=(const FChunkFormCoords2D& c) const {
		return X != c.X || Y != c.Y;
	}

	FChunkFormCoords2D operator+(const FChunkFormCoords2D& c) const {
		return { X + c.X, Y + c.Y };
	}
	FChunkFormCoords2D operator-(const FChunkFormCoords2D& c) const {
		return { X - c.X, Y - c.Y };
	}
	FChunkFormCoords2D operator*(int32 i) {
		return { X * i, Y * i };
	}

	FChunkFormCoords2D operator+=(const FChunkFormCoords2D& c) {
		return { X + c.X, Y + c.Y };
	}
	FChunkFormCoords2D operator-=(const FChunkFormCoords2D& c) {
		return { X - c.X, Y - c.Y };
	}

	virtual FString toString() override
	{
		return FString("X: ") + FString::FromInt(X) + " Y: " + FString::FromInt(Y);
	}
};

struct FChunkFormCoords3D : public IStringable {
public:
	int32 X, Y, Z;

	FChunkFormCoords3D() {}
	FChunkFormCoords3D(int32 X, int32 Y, int32 Z) : X(X), Y(Y), Z(Z) {}
	FChunkFormCoords3D(float X, float Y, float Z) : X((int32) X), Y((int32) Y), Z((int32) Z) {}
	FChunkFormCoords3D(const FChunkFormCoords2D& c, int32 Z) : X(c.X), Y(c.Y), Z(Z) {}

	static FChunkFormCoords3D FromNormalCoords(float x, float y, float z) {
		return { (int32) floorf(x / 16), (int32) floorf(y / 16), (int32) floorf(z / 16) };
	}

	FChunkFormCoords2D To2D() const {
		return FChunkFormCoords2D{ X, Y };
	}

	FVector ToWorldCoordinates() {
		return {X * 1600.f, Y * 1600.f, Z * 1600.f};
	}

	bool operator==(const FChunkFormCoords3D& c) const {
		return X == c.X && Y == c.Y && Z == c.Z;
	}
	bool operator!=(const FChunkFormCoords3D& c) const {
		return X != c.X || Y != c.Y;
	}

	FChunkFormCoords3D operator+(const FChunkFormCoords3D& c) const {
		return { X + c.X, Y + c.Y, Z + c.Z };
	}
	FChunkFormCoords3D operator-(const FChunkFormCoords3D& c) const {
		return { X - c.X, Y - c.Y, Z - c.Z };
	}

	FChunkFormCoords3D operator+=(const FChunkFormCoords3D& c) {
		return { X + c.X, Y + c.Y, Z + c.Z };
	}
	FChunkFormCoords3D operator-=(const FChunkFormCoords3D& c) {
		return { X - c.X, Y - c.Y, Z - c.Z };
	}
	FChunkFormCoords3D operator*(int32 i) {
		return {X * i, Y * i, Z * i};
	}

	virtual FString toString() override
	{
		return FString("X: ") + FString::FromInt(X) + " Y: " + FString::FromInt(Y) + " Z: " + FString::FromInt(Z);
	}
};

FORCEINLINE uint32 GetTypeHash(const FChunkFormCoords2D& c) {
	uint32 xhash = c.X % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.X % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8);
}

FORCEINLINE uint32 GetTypeHash(const FChunkFormCoords3D& c) {
	uint32 xhash = c.X % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.X % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8) && c.Z;
}

#pragma once

#include "CoreMinimal.h"

struct ChunkFormCoords2D {
	int32 X, Y;

	bool operator==(const ChunkFormCoords2D& c) const {
		return X == c.X && Y == c.Y;
	}
	bool operator!=(const ChunkFormCoords2D& c) const {
		return X != c.X || Y != c.Y;
	}

	ChunkFormCoords2D operator+(const ChunkFormCoords2D& c) const {
		return { X + c.X, Y + c.Y };
	}
	ChunkFormCoords2D operator-(const ChunkFormCoords2D& c) const {
		return { X - c.X, Y - c.Y };
	}
	ChunkFormCoords2D operator*(int32 i) {
		return { X * i, Y * i };
	}

	ChunkFormCoords2D operator+=(const ChunkFormCoords2D& c) {
		return { X + c.X, Y + c.Y };
	}
	ChunkFormCoords2D operator-=(const ChunkFormCoords2D& c) {
		return { X - c.X, Y - c.Y };
	}

};

struct ChunkFormCoords3D {
	int32 X, Y, Z;

	ChunkFormCoords3D() {}
	ChunkFormCoords3D(int32 X, int32 Y, int32 Z) : X(X), Y(Y), Z(Z) {}
	ChunkFormCoords3D(float X, float Y, float Z) : X((int32) X), Y((int32) Y), Z((int32) Z) {}
	ChunkFormCoords3D(const ChunkFormCoords2D& c, int32 Z) : X(c.X), Y(c.Y), Z(Z) {}

	static ChunkFormCoords3D FromNormalCoords(float x, float y, float z) {
		return { (int32) floorf(x / 16), (int32) floorf(y / 16), (int32) floorf(z / 16) };
	}

	ChunkFormCoords2D To2D() const {
		return ChunkFormCoords2D{ X, Y };
	}

	FVector ToWorldCoordinates() {
		return {X * 1600.f, Y * 1600.f, Z * 1600.f};
	}

	bool operator==(const ChunkFormCoords3D& c) const {
		return X == c.X && Y == c.Y && Z == c.Z;
	}
	bool operator!=(const ChunkFormCoords3D& c) const {
		return X != c.X || Y != c.Y;
	}

	ChunkFormCoords3D operator+(const ChunkFormCoords3D& c) const {
		return { X + c.X, Y + c.Y, Z + c.Z };
	}
	ChunkFormCoords3D operator-(const ChunkFormCoords3D& c) const {
		return { X - c.X, Y - c.Y, Z - c.Z };
	}

	ChunkFormCoords3D operator+=(const ChunkFormCoords3D& c) {
		return { X + c.X, Y + c.Y, Z + c.Z };
	}
	ChunkFormCoords3D operator-=(const ChunkFormCoords3D& c) {
		return { X - c.X, Y - c.Y, Z - c.Z };
	}
	ChunkFormCoords3D operator*(int32 i) {
		return {X * i, Y * i, Z * i};
	}
};

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords2D& c) {
	uint32 xhash = c.X % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.X % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8);
}

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords3D& c) {
	uint32 xhash = c.X % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.X % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8) && c.Z;
}

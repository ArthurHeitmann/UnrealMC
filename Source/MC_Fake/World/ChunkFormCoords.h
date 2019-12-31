#pragma once

#include "CoreMinimal.h"

struct ChunkFormCoords2D {
	int32 x, y;

	bool operator==(const ChunkFormCoords2D& c) const {
		return x == c.x && y == c.y;
	}
	bool operator!=(const ChunkFormCoords2D& c) const {
		return x != c.x || y != c.y;
	}

	ChunkFormCoords2D operator+(const ChunkFormCoords2D& c) const {
		return { x + c.x, y + c.y };
	}
	ChunkFormCoords2D operator-(const ChunkFormCoords2D& c) const {
		return { x - c.x, y - c.y };
	}
	ChunkFormCoords2D operator*(int32 i) {
		return { x * i, y * i };
	}

	ChunkFormCoords2D operator+=(const ChunkFormCoords2D& c) {
		return { x + c.x, y + c.y };
	}
	ChunkFormCoords2D operator-=(const ChunkFormCoords2D& c) {
		return { x - c.x, y - c.y };
	}

};

struct ChunkFormCoords3D {
	int32 x, y, z;

	ChunkFormCoords3D() {}
	ChunkFormCoords3D(int32 X, int32 Y, int32 Z) : x(X), y(Y), z(Z) {}
	ChunkFormCoords3D(float X, float Y, float Z) : x((int32) X), y((int32) Y), z((int32) Z) {}
	ChunkFormCoords3D(const ChunkFormCoords2D& c, int32 Z) : x(c.x), y(c.y), z(Z) {}

	static ChunkFormCoords3D FromNormalCoords(float x, float y, float z) {
		return { (int32) floorf(x / 16), (int32) floorf(y / 16), (int32) floorf(z / 16) };
	}

	ChunkFormCoords2D To2D() const {
		return ChunkFormCoords2D{ x, y };
	}

	FVector ToWorldCoordinates() {
		return {x * 1600.f, y * 1600.f, z * 1600.f};
	}

	bool operator==(const ChunkFormCoords3D& c) const {
		return x == c.x && y == c.y && z == c.z;
	}
	bool operator!=(const ChunkFormCoords3D& c) const {
		return x != c.x || y != c.y;
	}

	ChunkFormCoords3D operator+(const ChunkFormCoords3D& c) const {
		return { x + c.x, y + c.y, z + c.z };
	}
	ChunkFormCoords3D operator-(const ChunkFormCoords3D& c) const {
		return { x - c.x, y - c.y, z - c.z };
	}

	ChunkFormCoords3D operator+=(const ChunkFormCoords3D& c) {
		return { x + c.x, y + c.y, z + c.z };
	}
	ChunkFormCoords3D operator-=(const ChunkFormCoords3D& c) {
		return { x - c.x, y - c.y, z - c.z };
	}
	ChunkFormCoords3D operator*(int32 i) {
		return {x * i, y * i, z * i};
	}
};

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords2D& c) {
	uint32 xhash = c.x % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.x % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8);
}

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords3D& c) {
	uint32 xhash = c.x % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.x % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8) && c.z;
}

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

	ChunkFormCoords2D operator+=(const ChunkFormCoords2D& c) {
		return { x + c.x, y + c.y };
	}
	ChunkFormCoords2D operator-=(const ChunkFormCoords2D& c) {
		return { x - c.x, y - c.y };
	}

};

struct ChunkFormCoords3D {
	int32 x, y, z;

	ChunkFormCoords2D To2D() const {
		return ChunkFormCoords2D{ x, y };
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

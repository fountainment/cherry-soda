#ifndef _VOXELEXPERIMENT_VOXEL_WORLD_H_
#define _VOXELEXPERIMENT_VOXEL_WORLD_H_

#include "Voxel/Chunk.h"

#include <CherrySoda/Util/Math.h>
#include <CherrySoda/Util/STL.h>

class World
{
public:
	CHERRYSODA_ITERABLE(m_chunks);

	World(cherrysoda::Math::Vec3 basePosition = Vec3_Zero)
	: m_basePosition(basePosition)
	{
	}

	Chunk* LoadChunks();

	static constexpr int Size() { return static_cast<int>(ms_worldSize); }
	static constexpr int WorldBlockSize() { return Size() * Chunk::Size(); }
	static constexpr int ChunkAmount() { return Size() * Size() * Size(); }
	static constexpr int BlockAmount() { return ChunkAmount() * Chunk::BlockAmount(); }

	inline int GetChunkIndex(Chunk* chunk)
	{
		int index = static_cast<int>(chunk - GetChunks());
		return (index >= 0 && index < ChunkAmount()) ? index : -1;
	}

	inline Chunk* GetChunks() { return cherrysoda::STL::Data(m_chunks); }
	inline Chunk* GetChunk(const cherrysoda::Math::IVec3& v)
	{
		int index = GetChunkIndex(v);
		return index >= 0 ? GetChunks() + index : nullptr;
	}

	inline const Chunk* GetChunks() const { return cherrysoda::STL::Data(m_chunks); }
	inline const Chunk* GetChunk(const cherrysoda::Math::IVec3& v) const
	{
		int index = GetChunkIndex(v);
		return index >= 0 ? GetChunks() + index : nullptr;
	}

	Block* GetBlock(const cherrysoda::Math::IVec3& v, Chunk** chunkOut = nullptr)
	{
		constexpr int worldBlockSize = WorldBlockSize();
		if (v.x < 0 || v.x >= worldBlockSize || v.y < 0 || v.y >= worldBlockSize || v.z < 0 || v.z >=worldBlockSize) {
			return nullptr;
		}
		Chunk* chunk = GetChunk(v / Chunk::Size());
		if (chunk && chunkOut) *chunkOut = chunk;
		return chunk ? chunk->GetBlock(v % Chunk::Size()) : nullptr;
	}

	bool SetBlockType(const cherrysoda::Math::IVec3& v, Block::Type type)
	{
		Chunk* chunk = nullptr;
		Block* block = GetBlock(v, &chunk);
		if (block && block->m_type != type)
		{
			auto oldType = block->m_type;
			block->m_type = type;
			chunk->SetChanged();
			if (oldType == Block::Type::None || type == Block::Type::None) {
				chunk->NotifyChanged(v % Chunk::Size(), type);
			}
			return true;
		}
		return false;
	}

	cherrysoda::Math::IVec3 GetIndexOfBlockAt(const cherrysoda::Math::Vec3& v)
	{
		return cherrysoda::Math::IVec3(v - BasePosition());
	}

	void SetBlockType(int index, Block::Type type)
	{
		constexpr int worldBlockSize = WorldBlockSize();
		int x = index;
		int z = x % worldBlockSize;
		x /= worldBlockSize;
		int y = x % worldBlockSize;
		x /= worldBlockSize;
		SetBlockType(cherrysoda::Math::IVec3(x, y, z), type);
	}

	inline cherrysoda::Math::Vec3 BasePosition() const { return m_basePosition; }
	inline cherrysoda::Math::Vec3 GetChunkPosition(const cherrysoda::Math::IVec3& v) const { return BasePosition() + GetWorldChunkPosition(v); }
	inline cherrysoda::Math::Vec3 GetBlockPosition(const cherrysoda::Math::IVec3& v) const { return BasePosition() + cherrysoda::Math::Vec3(v); }

	inline cherrysoda::Math::AABB GetAABB() const { return { BasePosition(), BasePosition() + cherrysoda::Math::Vec3(static_cast<float>(WorldBlockSize())) }; }
	inline cherrysoda::Math::AABB GetChunkAABB(const cherrysoda::Math::IVec3& v) const
	{
		cherrysoda::Math::AABB ret = { Vec3_Zero, Vec3_Zero };
		auto ck = GetChunk(v);
		if (ck) ret = ck->GetAABB();
		return ret;
	}
	inline cherrysoda::Math::AABB GetBlockAABB(const cherrysoda::Math::IVec3& v) const { return { BasePosition() + cherrysoda::Math::Vec3(v), BasePosition() + cherrysoda::Math::Vec3(v) + Vec3_One }; }

	static inline const cherrysoda::Math::Vec3 GetWorldChunkPosition(const cherrysoda::Math::IVec3& v) { return cherrysoda::Math::Vec3(v * Chunk::Size()); }

private:
	cherrysoda::STL::Vector<Chunk> m_chunks;
	cherrysoda::Math::Vec3 m_basePosition;

	static inline int GetChunkIndex(const cherrysoda::Math::IVec3& v)
	{
		if (v.x < 0 || v.x >= Size() || v.y < 0 || v.y >= Size() || v.z < 0 || v.z >= Size()) return -1;
		return v.z * Size() * Size() + v.y * Size() + v.x;
	}

	static constexpr int ms_worldSize = 8;
};

#endif // _VOXELEXPERIMENT_VOXEL_WORLD_H_

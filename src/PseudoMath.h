#pragma once

namespace SH_COMP
{

	struct SHVec2
	{
		float x, y;
	};


	struct SHVec4
	{
		float x, y, z, w;
	};

	struct SHVec3
	{
		SHVec3()
			:x{ 0.f }, y{ 0.f }, z{ 0.f }
		{}

		SHVec3(SHVec4 const& rhs)
			:x{ rhs.x }, y{ rhs.y }, z{ rhs.z }
		{}

		SHVec3(float inx, float iny, float inz)
			:x{ iny }, y{ iny }, z{ inz }
		{}

		float x, y, z;
	};

	struct SHVec4i
	{
		uint32_t x, y, z, w;
	};

	struct SHMat4
	{
		float data[16];

		bool operator==(SHMat4 const& rhs) const
		{
			for (auto i {0}; i < 16; ++i)
			{
				if (data[i] != rhs.data[i])
				{
					return false;
				}
			}

			return true;
		}
	};

	using IndexType = uint32_t;

}
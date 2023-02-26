#pragma once

namespace SH_COMP
{

	struct SHVec2
	{
		float x, y;
	};

	struct SHVec3
	{
		float x, y, z;
	};

	struct SHVec4
	{
		float x, y, z, w;
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
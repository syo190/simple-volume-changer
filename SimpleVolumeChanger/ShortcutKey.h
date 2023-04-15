#pragma once
#include "Resource.h"
#include <cstdint>
#include <fstream>

namespace Data {
	constexpr int ArraySize = 3;
	/// <summary>
	/// ショートカットキーとして使用する最大3つまでの仮想キーリスト
	/// </summary>
	struct ShortucutKeyData {
		std::int32_t upVkArray[ArraySize] = { NULL };
		std::int32_t downVkArray[ArraySize] = { NULL };
		bool WriteShortcutKey(const char path[]);
		bool ReadShortcutKey(const char path[]);
	};
}


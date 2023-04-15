#pragma once
#include "Resource.h"
#include <cstdint>
#include <fstream>

namespace Data {
	constexpr int ArraySize = 3;
	/// <summary>
	/// �V���[�g�J�b�g�L�[�Ƃ��Ďg�p����ő�3�܂ł̉��z�L�[���X�g
	/// </summary>
	struct ShortucutKeyData {
		std::int32_t upVkArray[ArraySize] = { NULL };
		std::int32_t downVkArray[ArraySize] = { NULL };
		bool WriteShortcutKey(const char path[]);
		bool ReadShortcutKey(const char path[]);
	};
}


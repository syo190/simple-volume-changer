#include "pch.h"
#include "ShortcutKey.h"

using namespace Data;

// ¬Œ÷‚·‚ê‚Î–ß‚è’ltrue
bool ShortucutKeyData::WriteShortcutKey(const char path[]) {
	std::ofstream ofs;
	ofs.open(path, std::ios::out | std::ios::binary);
	if (!ofs) return false;

	ofs.write(reinterpret_cast<const char*>(this), sizeof(*this));
	if (ofs.fail()) return false;
	ofs.close();
	return true;
};

// ¬Œ÷‚·‚ê‚Î–ß‚è’ltrue
bool ShortucutKeyData::ReadShortcutKey(const char path[]) {
	std::ifstream ifs;
	ifs.open(path, std::ios::in | std::ios::binary);
	if (!ifs) return false;

	ifs.read(reinterpret_cast<char*>(this), sizeof(*this));
	if (ifs.fail()) return false;
	ifs.close();

	return  true;
};

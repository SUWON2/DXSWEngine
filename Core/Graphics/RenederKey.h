#pragma once

// HACK: 무슨 기능을 하는지 명시하자
class RenederKey
{
	friend class Reneder;

private:
	RenederKey() = delete;

	RenederKey(const RenederKey&) = default;

	RenederKey& operator=(const RenederKey&) = delete;

	~RenederKey() = default;
};
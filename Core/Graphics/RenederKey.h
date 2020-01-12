#pragma once

class RenederKey
{
	friend class Reneder;

private:
	RenederKey() = delete;

	(RenederKey)(const RenederKey&) = default;

	RenederKey& operator=(const RenederKey&) = delete;

	~RenederKey() = default;
};
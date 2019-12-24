#pragma once

class Reneder final
{
public:
	Reneder();

	Reneder(const Reneder&) = default;

	Reneder& operator=(const Reneder&) = delete;

	~Reneder();

	void Draw();
};
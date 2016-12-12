#pragma once
#include <exception>

class InvalidOp : public std::exception
{
	const char* what() const noexcept override { return "Invalid OP code or parameters."; }
};

class FileLoadExcept : public std::exception
{
	const char* what() const noexcept override { return "ZOM file load exception"; }
};

class LineMissing : public std::exception
{
	const char* what() const noexcept override { return "Line missing for branch instruction"; }
};

class InvalidRangedAttack : public std::exception
{
	const char* what() const noexcept override { return "Zombies cannot use ranged attacks"; }
};

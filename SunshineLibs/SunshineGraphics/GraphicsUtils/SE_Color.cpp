

#include "SE_Color.h"

SE_Color::SE_Color()
	:color(0)
{
}

SE_Color::SE_Color(unsigned int val)
	: color(val)
{
}

SE_Color::SE_Color(BYTE r, BYTE g, BYTE b)
	: SE_Color(r, g, b, 255)
{
}

SE_Color::SE_Color(BYTE r, BYTE g, BYTE b, BYTE a)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

SE_Color::SE_Color(const SE_Color& src)
	:color(src.color)
{
}

SE_Color& SE_Color::operator=(const SE_Color& src)
{
	this->color = src.color;
	return *this;
}

bool SE_Color::operator==(const SE_Color& rhs) const
{
	return (this->color == rhs.color);
}

bool SE_Color::operator!=(const SE_Color& rhs) const
{
	return !(*this == rhs);
}

constexpr BYTE SE_Color::GetR() const
{
	return this->rgba[0];
}
void SE_Color::SetR(BYTE r)
{
	this->rgba[0] = r;
}

constexpr BYTE SE_Color::GetG() const
{
	return this->rgba[1];
}
void SE_Color::SetG(BYTE g)
{
	this->rgba[1] = g;
}

constexpr BYTE SE_Color::GetB() const
{
	return this->rgba[2];
}
void SE_Color::SetB(BYTE b)
{
	this->rgba[2] = b;
}

constexpr BYTE SE_Color::GetA() const
{
	return this->rgba[3];
}
void SE_Color::SetA(BYTE a)
{
	this->rgba[3] = a;
}
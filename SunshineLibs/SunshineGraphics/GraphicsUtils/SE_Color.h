#pragma once



typedef unsigned char BYTE; // 8 bits

class SE_Color
{
public:
	SE_Color();
	SE_Color(unsigned int val);
	SE_Color(BYTE r, BYTE g, BYTE b);
	SE_Color(BYTE r, BYTE g, BYTE b, BYTE a);
	SE_Color(const SE_Color& src);

	SE_Color& operator=(const SE_Color& src);
	bool operator==(const SE_Color& rhs) const;
	bool operator!=(const SE_Color& rhs) const;

	constexpr BYTE GetR() const;
	void SetR(BYTE r);

	constexpr BYTE GetG() const;
	void SetG(BYTE g);

	constexpr BYTE GetB() const;
	void SetB(BYTE b);

	constexpr BYTE GetA() const;
	void SetA(BYTE a);

private:
	union
	{
		BYTE rgba[4];
		unsigned int color;
	};
};

namespace SE_Colors
{
	const SE_Color UnloadedTextureColor(100, 100, 100);
	const SE_Color UnhandledTextureColor(250, 0, 0);
}
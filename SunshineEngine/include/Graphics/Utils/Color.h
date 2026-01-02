#pragma once



typedef unsigned char BYTE; // 8 bits

namespace SE_G {
	class Color
	{
	public:
		Color();
		Color(unsigned int val);
		Color(BYTE r, BYTE g, BYTE b);
		Color(BYTE r, BYTE g, BYTE b, BYTE a);
		Color(const Color& src);

		Color& operator=(const Color& src);
		bool operator==(const Color& rhs) const;
		bool operator!=(const Color& rhs) const;

		constexpr BYTE GetR() const;
		void SetR(BYTE r);

		constexpr BYTE GetG() const;
		void SetG(BYTE g);

		constexpr BYTE GetB() const;
		void SetB(BYTE b);

		constexpr BYTE GetA() const;
		void SetA(BYTE a);

	//private:
		union
		{
			BYTE rgba[4];
			unsigned int color;
		};
	};

	namespace Colors
	{
		const Color UnloadedTextureColor(250, 0, 250);
		const Color UnhandledTextureColor(250, 0, 0);
	}
}

#include<Windows.h>
#include<string>
using namespace std;

class ScreenPixel
{
	COLORREF color;
	HDC hDC;
	BOOL b;

public :
	ScreenPixel()
	{
		hDC = GetDC(NULL);
		if (hDC == NULL)
			throw string("exception : hdc is null");
	}

	int getFrom(const POINT& pt)
	{
		return GetPixel(hDC, pt.x, pt.y);
	}

	~ScreenPixel()
	{
		ReleaseDC(GetDesktopWindow(), hDC);
	}
};
#pragma once
class CUKDib : public CObject
{
public:
	CUKDib(void);
	~CUKDib(void);

	BOOL Open(const char* pzFileName = NULL);
	void Close();
	void Draw(CDC* pDC, int x=0, int y=0);

private:
	BYTE*	m_pDib;
	BITMAPFILEHEADER*	m_pBMFH;
	BITMAPINFOHEADER*	m_pBMIH;
	BITMAPINFO*	m_pBMI;
	BYTE*	m_pData;
};


#ifndef VOLUME_DEMO_WINDOW_H__
#define VOLUME_DEMO_WINDOW_H__

#include <windows.h>

class OSWindow
{
public:
	OSWindow();
	~OSWindow();

	bool Init(int w, int h);

	bool Show();

	bool Close();

	bool CreateOglContext();
	bool MakeCurrentContext();
	bool RemoveContext();
	bool Swap() const;


private:

	void * _handle;

	HGLRC _oglContext;

	HDC _hdc;
};

#endif

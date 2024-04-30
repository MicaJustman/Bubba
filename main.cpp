#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <ctime>
#include <numeric>
#include <Windows.h>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat getMat(HWND hwnd, int width, int height, int offset_width, int offset_height) {
	HDC deviceContext = GetDC(hwnd);
	HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);
	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);
	SelectObject(memoryDeviceContext, bitmap);
	BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, offset_width, offset_height, SRCCOPY);
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 0;

	Mat mat = Mat(height, width, CV_8UC4);
	GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(bitmap);
	DeleteDC(memoryDeviceContext);
	ReleaseDC(hwnd, deviceContext);

	return mat;
}

void click() {
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(50);

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}

int main() {
	Mat frame, gray, grayScreen, grayBase, result;
	double minVal; double maxVal; Point minLoc; Point maxLoc;

	//LPCWSTR windowTitle = L"DeadByDaylight  ";
	//HWND hwnd = FindWindow(NULL, windowTitle);

	HWND hwnd = GetDesktopWindow();
	RECT windowRect;
	GetClientRect(hwnd, &windowRect);

	int width = windowRect.right;
	int height = windowRect.bottom;

	grayBase = imread("template.png", IMREAD_GRAYSCALE);

	do {
		cvtColor(getMat(hwnd, width, height, 0, 0), grayScreen, COLOR_BGR2GRAY);
		result.create(grayScreen.cols, grayScreen.rows, CV_32FC1);
		matchTemplate(grayScreen, grayBase, result, TM_CCOEFF_NORMED);
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		cout << maxVal;
	} while (maxVal < .7);

	while (true) {
		frame = getMat(hwnd, grayBase.cols, grayBase.rows, maxLoc.x, maxLoc.y);
		uchar* framedata = frame.data;

		int reds = 0;

		for (int row = 0; row < frame.rows; ++row) {
			for (int col = 0; col < frame.cols; ++col) {
				if (framedata[(row * frame.rows + col) * 4] < 50 and framedata[(row * frame.rows + col) * 4 + 1] < 50 and framedata[(row * frame.rows + col) * 4 + 2] > 200) {
					reds++;
				}
			}
		}

		cout << reds << endl;
		if (10 < reds and reds < 90 and GetAsyncKeyState(VK_LBUTTON) < 0) {
			click();
			Sleep(400);
		}
	}

	
	return 0;
}
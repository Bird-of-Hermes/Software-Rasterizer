#include "Renderer.hpp"
#include <limits>

Application::~Application()
{
	for (size_t i = 0; i < BACKBUFFERCOUNT; ++i)
	{
		Allocator::Free(reinterpret_cast<void*&>(m_backBuffers[i]));
		Allocator::Free(reinterpret_cast<void*&>(m_alphaChannel[i]));
	}
	if (m_windowContext->m_hwnd && m_windowContext->m_hdc)
	{
		ReleaseDC(m_windowContext->m_hwnd, m_windowContext->m_hdc);
	}
	Allocator::Free(reinterpret_cast<void*&>(m_accumulationBuffer));
}

RESULT_VALUE Application::Start(uint16_t width, uint16_t height, std::wstring_view windowName, size_t bytesPrealloc, size_t maxManagedObjects, size_t alignment) noexcept
{
	using depthBufferType = std::remove_pointer_t<decltype(m_depthBuffer)>;

	canvasWidth = (width < 320) ? 320 : alignValue(width, 4);
	canvasHeight = (height < 240) ? 240 : alignValue(height, 4);

	m_windowContext = std::make_unique<Platform::Window>((uint16_t)canvasWidth, (uint16_t)canvasHeight, windowName.data());

	VPMatrix = ViewPortMatrix(canvasWidth, canvasHeight);

	{	// Pre-allocation
		const size_t backBuffersSize = size_t(width) * height * sizeof(Color) * BACKBUFFERCOUNT;
		const size_t alphaChannelSize = size_t(width) * height * sizeof(unsigned char) * BACKBUFFERCOUNT;
		const size_t depthBufferSize = size_t(width) * height * sizeof(depthBufferType);
		const size_t accumulationBufferSize = size_t(width) * height * sizeof(uint32_t);

		size_t toAllocate = backBuffersSize + alphaChannelSize + depthBufferSize + accumulationBufferSize;
		// reserve
		toAllocate += (bytesPrealloc == 0 ? MB(30) : bytesPrealloc);

		Allocator::Init(toAllocate, alignment < 64 ? 64 : alignment, maxManagedObjects < 4096 ? 4096 : maxManagedObjects);
	}
	OnInit();
	CreateBackBuffers();

	return Loop();
}

template <typename ...Args>
void Application::DrawPixelShader(const std::function<Color(uint16_t, uint16_t, Args...)>& shader, Args&&... params) noexcept
{
	for (uint16_t y = 0; y < canvasHeight; y++)
	{
		for (uint16_t x = 0; x < canvasWidth; x++)
		{
			const Color color = shader(x, y, params);
			DrawPixel(x, y, color);
		}
	}
}

void Application::DrawPixelShader(const std::function<Color(uint16_t, uint16_t)>& shader) noexcept
{
	for (uint16_t y = 0; y < canvasHeight; y++)
	{
		for (uint16_t x = 0; x < canvasWidth; x++)
		{
			const Color color = shader(x, y);
			DrawPixel(x, y, color);
		}
	}
}

void Application::DrawImage(uint16_t x, uint16_t y, Image* img, float xScale, float yScale, bool invertX, bool invertY) noexcept
{
	if (x >= canvasWidth || y >= canvasHeight || xScale <= 0.0f || yScale <= 0.0f) [[unlikely]]
	{
		return;
	}

	const uint16_t imgWidth = static_cast<uint16_t>(img->width * xScale);
	const uint16_t imgHeight = static_cast<uint16_t>(img->height * yScale);

	// canvas
	const uint16_t endX = (size_t(x) + imgWidth) > canvasWidth ? (uint16_t)canvasWidth :    uint16_t(x + imgWidth);
	const uint16_t endY = (size_t(y) + imgHeight) > canvasHeight ? (uint16_t)canvasHeight : uint16_t(y + imgHeight);

	// img 
	float imgX = invertX ? (imgWidth - 1) / xScale : 0;
	float imgY = invertY ? (imgHeight - 1) / yScale : 0;
	const float xStep = invertX ? -1.0f / xScale : 1.0f / xScale;
	const float yStep = invertY ? -1.0f / yScale : 1.0f / yScale;
	const float startX = imgX;

	Color*& presentBBuffer = m_backBuffers[presentBufferIndex];
	Color*& imgBuffer = img->pixelGrid;

	for (uint16_t j = y; j < endY; j++, imgY += yStep)
	{
		const size_t BBindex = j * canvasWidth;
		const size_t imgIndex = (size_t)imgY * img->width;
		for (uint16_t i = x; i < endX; i++, imgX += xStep)
		{
			memcpy(&presentBBuffer[BBindex + i], &imgBuffer[imgIndex + (size_t)imgX], sizeof(Color));
		}
		imgX = startX;
	}
}

void Application::DrawPixel(uint16_t x, uint16_t y, uint8_t Red, uint8_t Green, uint8_t Blue, size_t currentSampleIndex) noexcept
{
	if (x >= canvasWidth || y >= canvasHeight) [[unlikely]]
	{
		return;
	}
	if (currentSampleIndex > 1)
	{
		DrawPixelAccumulate(x, y, Red, Green, Blue, currentSampleIndex);
		return;
	}
	const size_t index = static_cast<size_t>(y) * canvasWidth + x;
	const Color color = Color(Red, Green, Blue);

	memcpy(&m_backBuffers[presentBufferIndex][index], &color, sizeof(Color));
}

void Application::DrawPixel(uint16_t x, uint16_t y, Color rgb, size_t currentSampleIndex) noexcept
{
	if (x >= canvasWidth || y >= canvasHeight) [[unlikely]]
	{
		return;
	}
	if (currentSampleIndex > 1)
	{
		DrawPixelAccumulate(x, y, rgb, currentSampleIndex);
		return;
	}
	const size_t index = static_cast<size_t>(y) * canvasWidth + x;

	memcpy(&m_backBuffers[presentBufferIndex][index], &rgb, sizeof(Color));
}

void Application::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color rgb) noexcept
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;

	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		m_depthBuffer[y * canvasWidth + x];
		DrawPixel((uint16_t)x, (uint16_t)y, rgb);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
			{
				px = px + 2 * dy1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (dy1 - dx1);
			}
			DrawPixel((uint16_t)x, (uint16_t)y, rgb);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		DrawPixel((uint16_t)x, (uint16_t)y, rgb);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			DrawPixel((uint16_t)x, (uint16_t)y, rgb);
		}
	}
}

void Application::DrawLine(const Vec2f& p0, const Vec2f& p1, Color rgb) noexcept
{
	int x1 = { int(p0.x) };
	int x2 = { int(p1.x) };
	int y1 = { int(p0.y) };
	int y2 = { int(p1.y) };
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; 
	dy = y2 - y1;
	dx1 = abs(dx); 
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;	
	py = 2 * dx1 - dy1;

	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		DrawPixel((uint16_t)x, (uint16_t)y, rgb);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			DrawPixel((uint16_t)x, (uint16_t)y, rgb);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		DrawPixel((uint16_t)x, (uint16_t)y, rgb);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			DrawPixel((uint16_t)x, (uint16_t)y, rgb);
		}
	}
}

void Application::DrawLine(const Vec3f& p0, const Vec3f& p1, Color rgb) noexcept
{
	using namespace std;
	using depthBufferType = remove_pointer_t<decltype(m_depthBuffer)>;

	static const float max = static_cast<float>(numeric_limits<depthBufferType>::max());
	const depthBufferType Zvalue = static_cast<depthBufferType>(abs(p0.z * max));

	static auto drawPixel = [&](int x, int y) noexcept -> void
		{
			if ((x < canvasWidth && y < canvasHeight))
			{
				const size_t index = static_cast<size_t>(y) * canvasWidth + x;
				if (m_depthBuffer[index] > Zvalue)
				{
					// write into z-buffer
					m_depthBuffer[index] = Zvalue;// = Zvalue;

					// write into frame-buffer
					memcpy(&m_backBuffers[presentBufferIndex][index], &rgb, sizeof(Color));
				}
			}
		};

	int32_t x1 = static_cast<int>(floor(p0.x + 0.5f));
	int32_t x2 = static_cast<int>(floor(p1.x + 0.5f));
	int32_t y1 = static_cast<int>(floor(p0.y + 0.5f));
	int32_t y2 = static_cast<int>(floor(p1.y + 0.5f));

	// Verify if it's a subpixel line
	if (abs(x2 - x1) < 1 && abs(y2 - y1) < 1)
	{
		return;
	}

	// Draw from the top to the bottom
	if (y1 > y2)
	{
		swap(y1, y2);
		swap(x1, x2);
	}

	int32_t dx = abs(x2 - x1);
	int32_t dy = abs(y2 - y1);
	int32_t sx = x1 < x2 ? 1 : -1;
	int32_t sy = y1 < y2 ? 1 : -1;
	int32_t err = (dx > dy ? dx : -dy) / 2;
	int32_t e2;

	while (true)
	{
		drawPixel(x1, y1);
		if (x1 == x2 && y1 == y2)
		{
			break;
		}
		e2 = err;
		if (e2 > -dx) 
		{ 
			err -= dy; 
			x1 += sx; 
		}
		if (e2 < dy) 
		{ 
			err += dx; 
			y1 += sy; 
		}
	}
}

void Application::SetWindowTitle(std::wstring_view name) const noexcept
{
	SetWindowTextW(m_windowContext->m_hwnd, name.data());
}

void Application::SetWindowTitle(std::string_view name) const noexcept
{
	SetWindowTextA(m_windowContext->m_hwnd, name.data());
}

void Application::ClearScreenToogle(bool value) noexcept
{
	m_clearScreen = value;
}

void Application::InvertYaxis(bool value) noexcept
{
	m_InvertYaxis = value;
}

void Application::CreateBackBuffers()
{
	const size_t canvasSize = canvasWidth * canvasHeight;

	// separate the loops so the assigned memory is contiguous
	// [for raytracing] first since it's the least accessed
	logResult(Allocator::Allocate(reinterpret_cast<void*&>(m_accumulationBuffer), canvasSize * sizeof(uint32_t)));

	// [for alpha blending] 2nd since alphaChannel is rarely used
	for (size_t i = 0; i < BACKBUFFERCOUNT; i++)
	{
		if (m_alphaChannel[i] == nullptr)
		{
			// A
			logResult(Allocator::Allocate(reinterpret_cast<void*&>(m_alphaChannel[i]), canvasSize * sizeof(unsigned char)));
		}
	}

	// 3rd is the main frame buffer
	for (size_t i = 0; i < BACKBUFFERCOUNT; i++)
	{
		if (m_backBuffers[i] == nullptr)
		{
			// RGB
			logResult(Allocator::Allocate(reinterpret_cast<void*&>(m_backBuffers[i]), canvasSize * sizeof(Color)));
		}
	}

	// will always access it also
	logResult(Allocator::Allocate(reinterpret_cast<void*&>(m_depthBuffer), canvasSize * sizeof(unsigned char)));
}

void Application::DrawPixelAccumulate(uint16_t x, uint16_t y, Color rgb, size_t currentSampleIndex) noexcept
{
	const size_t index = (static_cast<size_t>(y) * canvasWidth + x) * 3;
	m_accumulationBuffer[index] += rgb.red;
	m_accumulationBuffer[index + 1] += rgb.green;
	m_accumulationBuffer[index + 2] += rgb.blue;

	const Color color = Color
	(
		static_cast<unsigned char>(m_accumulationBuffer[index + 2] / (currentSampleIndex)), // red
		static_cast<unsigned char>(m_accumulationBuffer[index + 1] / (currentSampleIndex)), // green
		static_cast<unsigned char>(m_accumulationBuffer[index] / (currentSampleIndex))      // blue
	);

	memcpy(&m_backBuffers[presentBufferIndex][index / 3], &color, sizeof(Color));
	presentSampleIndex = currentSampleIndex;
}

void Application::DrawPixelAccumulate(uint16_t x, uint16_t y, uint8_t red, uint8_t green, uint8_t blue, size_t currentSampleIndex) noexcept
{
	const size_t index = (static_cast<size_t>(y) * canvasWidth + x) * 3;
	m_accumulationBuffer[index] += red;
	m_accumulationBuffer[index + 1] += green;
	m_accumulationBuffer[index + 2] += blue;

	const Color color = Color
	(
		static_cast<unsigned char>(m_accumulationBuffer[index + 2] / (currentSampleIndex)), // red
		static_cast<unsigned char>(m_accumulationBuffer[index + 1] / (currentSampleIndex)), // green
		static_cast<unsigned char>(m_accumulationBuffer[index] / (currentSampleIndex))      // blue
	);

	memcpy(&m_backBuffers[presentBufferIndex][index / 3], &color, sizeof(Color));
	presentSampleIndex = currentSampleIndex;
}

void Application::Present() noexcept
{
	const BITMAPINFO bmi = 
	{
		.bmiHeader = {.biSize = sizeof(bmi.bmiHeader),
		.biWidth = static_cast<LONG>(canvasWidth),
		.biHeight = static_cast<LONG>(m_InvertYaxis ? static_cast<int16_t>(canvasHeight) : -static_cast<int16_t>(canvasHeight)),
		.biPlanes = 1,
		.biBitCount = 24,
		.biCompression = BI_RGB
	}};

	SetDIBitsToDevice
	(
		m_windowContext->m_hdc, 
		0, 
		0, 
		(int16_t)canvasWidth,
		(int16_t)canvasHeight,
		0, 
		0, 
		0, 
		(int16_t)canvasHeight,
		m_backBuffers[presentBufferIndex], 
		&bmi, 
		DIB_RGB_COLORS
	);

	presentBufferIndex = (presentBufferIndex + 1) % BACKBUFFERCOUNT;
}

void Application::ClearScreen() const noexcept
{
	using depthBufferType = std::remove_pointer_t<decltype(m_depthBuffer)>;

	static size_t lastSampleIndex = presentSampleIndex;
	const size_t canvasSize = (size_t)canvasWidth * canvasHeight;

	memset(m_backBuffers[presentBufferIndex], 0x4D, canvasSize * sizeof(Color)); //  gray-ish
	memset(m_depthBuffer, 0xFF, canvasSize * sizeof(depthBufferType));
	
	// clear accumulation buffer only if the current sample N is lower than the last update (in case camera moved etc... -> for static image raytracing)
	if (presentSampleIndex < lastSampleIndex)
	{
		memset(m_accumulationBuffer, 0, canvasSize * sizeof(uint32_t));
	}
	lastSampleIndex = presentSampleIndex;
}

RESULT_VALUE Application::Loop()
{
	auto last = std::chrono::high_resolution_clock::now();
	MSG msg = {};

	float accumulatedTime = 0.0;
	size_t frameCount = 0;

	while (true)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) [[unlikely]]
			{
				return RESULT_VALUE::OK;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		const auto now = std::chrono::high_resolution_clock::now();
		const auto elapsed = now - last;
		last = now;
		const float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() / 1000.0f;

		accumulatedTime += deltaTime;

		if (accumulatedTime > 0.25f)
		{
			currentFPS = static_cast<size_t>((float)frameCount / accumulatedTime);
			accumulatedTime = 0.0;
			frameCount = 0;
		}

		if (m_clearScreen)
		{
			ClearScreen();
		}
		OnUpdate(deltaTime);
		Present();

		++frameCount;
		++frameIndex;
	}

	return RESULT_VALUE::OK;
}

#ifndef RT_RENDERER_HPP
#define RT_RENDERER_HPP

#include "Window.hpp"
#include "ErrorEnum.hpp"
#include "Object3D.hpp"
#include "Illumination.hpp"
#include "Clipping.hpp"
#include "Cameras.hpp"
#include <memory>
#include <chrono>
#include <functional>
#include <future>

class Application
{
	static constexpr size_t BACKBUFFERCOUNT = 2;

public:
	Application() {}
	virtual ~Application();

	// if (dt > 1.0f) to check if a full second has passed
	virtual void OnUpdate(float dt) noexcept = 0;
	virtual void OnInit() = 0;

	// If gonna change the memory to be allocated, use the MB() / GB() functions for easiness
	// The first 3 parameters define the window configuration. The title can be changed at any time through SetWindowTitle() but the screen width and height are fixed;
	// Width or height that's below the default will be ignored, and any value will be aligned to 4, i.e. a width set to 737 will turn into 740;
	// For maxManagedObjects and alignment, any value below the defaults are discarted, all in all you shouldn't need to change those but they're available nonetheless.
	RESULT_VALUE Start(uint16_t WindowWidth = 320, uint16_t WindowHeight = 240, std::wstring_view windowDefaultName = L"My Application", size_t bytesPrealloc = MB(30), size_t maxManagedObjects = 4096, size_t alignment = 64) noexcept;

	template <typename... Args>
	void DrawPixelShader(const std::function<Color(uint16_t, uint16_t, Args...)>& shader, Args&&... params) noexcept;
	void DrawPixelShader(const std::function<Color(uint16_t, uint16_t)>& shader) noexcept;

	void DrawImage(uint16_t x, uint16_t y, Image* img, float xScale = 1.0f, float yScale = 1.0f, bool invertX = false, bool invertY = false) noexcept;
	void DrawPixel(uint16_t x, uint16_t y, uint8_t red, uint8_t green, uint8_t blue, size_t currentSampleIndex = 1) noexcept;
	void DrawPixel(uint16_t x, uint16_t y, Color rgb, size_t currentSampleIndex = 1) noexcept;
	void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color rgb) noexcept;
	void DrawLine(const Vec2f& p1, const Vec2f& p2, Color rgb) noexcept;

	template <minVertex vertexType = Vertex>
	void Draw3DObject(const Object3D<vertexType>& object, const Camera& camera, bool wireframe = false) noexcept;

	void SetWindowTitle(std::wstring_view name) const noexcept;
	void SetWindowTitle(std::string_view name) const noexcept;
	void ClearScreenToogle(bool value) noexcept;
	void InvertYaxis(bool value) noexcept;

	constexpr size_t CanvasWidth() const noexcept { return canvasWidth; }
	constexpr size_t CanvasHeight() const noexcept { return canvasHeight; }
	constexpr size_t FrameIndex() const noexcept { return frameIndex; }
	constexpr size_t FPS() const noexcept { return currentFPS; }

private:
	void CreateBackBuffers();
	void Present() noexcept;
	void ClearScreen() const noexcept;
	RESULT_VALUE Loop();

	// mainly for raytracing
	void DrawPixelAccumulate(uint16_t x, uint16_t y, Color rgb, size_t currentSampleIndex) noexcept;
	void DrawPixelAccumulate(uint16_t x, uint16_t y, uint8_t red, uint8_t green, uint8_t blue, size_t currentSampleIndex) noexcept;

	// used exclusively inside Draw3DObject
	template <minVertex vertexType = Vertex>
	void DrawTriangle(const Triangle<vertexType>& triangle) noexcept;

	template <minVertex vertexType = Vertex>
	void DrawTexturedTriangle(const Triangle<vertexType>& triangle, const Image& texture) noexcept;
	void DrawLine(const Vec3f& p0, const Vec3f& p1, Color rgb) noexcept;

private:
	// Utility
	size_t canvasWidth = 800;
	size_t canvasHeight = 600;
	size_t currentFPS = 0;
	size_t frameIndex = 1;
	Matrix4x4f VPMatrix;

private:

	// Content Window
	std::unique_ptr<Platform::Window> m_windowContext = {nullptr};

	// Buffers
	uint32_t* m_accumulationBuffer = nullptr;
	Color* m_backBuffers[BACKBUFFERCOUNT] = { nullptr };
	unsigned char* m_alphaChannel[BACKBUFFERCOUNT] = { nullptr };
	unsigned short* m_depthBuffer = nullptr;
	
	size_t presentBufferIndex = 0;
	size_t presentSampleIndex = 1;

	// configurations
	bool m_clearScreen = true;
	bool m_InvertYaxis = false;
};

// could hide it away in the .cpp file, but have to lose the templated vertex and implement the same thing that DX12 does with D3D12_INPUT_ELEMENT_DESC + compiling and dlls
template <minVertex vertexType>
void Application::Draw3DObject(const Object3D<vertexType>& object, const Camera& camera, bool wireframe) noexcept
{
	const Matrix4x4f world = SRT
	(
		Scale(object.scale.x, object.scale.y, object.scale.z),
		Rotate(object.rotation.z, object.rotation.y, object.rotation.x),
		Translate(object.positionInSpace.x, object.positionInSpace.y, object.positionInSpace.z)
	);
	const Matrix4x4f worldView = world * camera.lastCameraMatrix;
	const Matrix4x4f projectionViewPort = ProjectionMatrix(
		(uint16_t)canvasWidth,
		(uint16_t)canvasHeight,
		camera.projection.fieldOfView,
		camera.projection.nearPlane,
		camera.projection.farPlane) * VPMatrix;

	const float canvasH = static_cast<float>(canvasHeight - 1);
	const float canvasW = static_cast<float>(canvasWidth - 1);

	for (size_t i = 0; i < object.meshArr.size(); i++)
	{
		const Mesh<vertexType>& mesh = object.meshArr[i];
		for (size_t j = 0; j < mesh.indices.size(); j += 3) // 3 vertices make a triangle
		{
			Triangle<vertexType> toWorld{ mesh.vertices[mesh.indices[j]], mesh.vertices[mesh.indices[j + 1]], mesh.vertices[mesh.indices[j + 2]] };
			toWorld *= world;

			const Vec3f normal = normalize(cross(toWorld.b.position - toWorld.a.position, toWorld.c.position - toWorld.a.position));

			// If ray is aligned with normal, then triangle is visible
			if (dot(normal, toWorld.a.position - camera.position) < 0.0f)
			{
				// world to View, clip against near plane
				ClippedTriangle<vertexType> nearClipped = ClipAgainstPlane<vertexType>({ 0.0f, 0.0f, camera.projection.nearPlane }, { 0.0f, 0.0f, 1.0f }, toWorld * camera.lastCameraMatrix);

				std::array<Triangle<vertexType>, 16> tArray;
				size_t triangleCount = 0;

				for (size_t n = 0; n < nearClipped.num; n++)
				{
					// clip against far plane
					const ClippedTriangle<vertexType> farClipped = ClipAgainstPlane<vertexType>({ 0.0f, 0.0f, camera.projection.farPlane }, { 0.0f, 0.0f, -1.0f }, nearClipped.triangles[n]);

					for (size_t m = 0; m < farClipped.num; m++)
					{
						// apply projection and scale to view port
						tArray[triangleCount++] = farClipped.triangles[m] * projectionViewPort;
					}
				}

				size_t newTriangles = triangleCount;

				for (size_t p = 0; p < 4; p++)
				{
					size_t arrayIterator = 0;
					size_t newCount = 0;

					while (newTriangles-- > 0)
					{
						const Triangle<vertexType>& front = tArray[arrayIterator++];

						ClippedTriangle<vertexType> planeRes;

						// clip against every other plane
						switch (p)
						{
							case 0: planeRes = ClipAgainstPlane<vertexType>({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, front); break;
							case 1: planeRes = ClipAgainstPlane<vertexType>({ 0.0f, canvasH, 0.0f }, { 0.0f, -1.0f, 0.0f }, front); break;
							case 2: planeRes = ClipAgainstPlane<vertexType>({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, front); break;
							case 3: planeRes = ClipAgainstPlane<vertexType>({ canvasW, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, front); break;
						}

						for (size_t n = 0; n < planeRes.num; n++)
						{
							tArray[newCount++] = planeRes.triangles[n];
						}
					}

					arrayIterator = 0;
					triangleCount = newCount;
					newTriangles = newCount;
				}

				// Finally draw it
				if (!wireframe)
				{
					for (size_t t = 0; t < triangleCount; t++)
					{
						DrawTexturedTriangle(tArray[t], object.diffuseTextures[i]);
					}
				}
				else
				{
					for (size_t t = 0; t < triangleCount; t++)
					{
						DrawTriangle(tArray[t]);
					}
				}
			}
		}
	}
}

template <minVertex vertexType>
void Application::DrawTriangle(const Triangle<vertexType>& triangle) noexcept
{
	DrawLine(triangle.a.position, triangle.b.position, { (unsigned char)255, (unsigned char)255, (unsigned char)255 });
	DrawLine(triangle.b.position, triangle.c.position, { (unsigned char)255, (unsigned char)255, (unsigned char)255 });
	DrawLine(triangle.c.position, triangle.a.position, { (unsigned char)255, (unsigned char)255, (unsigned char)255 });
}

template <minVertex vertexType>
void Application::DrawTexturedTriangle(const Triangle<vertexType>& triangle, const Image& texture) noexcept
{
	using namespace std;
	using depthBufferType = remove_pointer_t<decltype(m_depthBuffer)>;

	static const float max = static_cast<float>(numeric_limits<depthBufferType>::max());

	auto drawPixel = [&](uint16_t x, uint16_t y, float u, float v, float z) noexcept -> void
		{
			const size_t index = static_cast<size_t>(y) * canvasWidth + x;
			depthBufferType Zvalue = static_cast<depthBufferType>(abs(z * max));

			if (m_depthBuffer[index] > Zvalue)
			{
				m_depthBuffer[index] = Zvalue;
				const Color rgb = texture.sample(u, v);
				memcpy(&m_backBuffers[presentBufferIndex][index], &rgb, sizeof(Color));
			}
		};

	// Bring data to the stack
	int32_t x1 = static_cast<int32_t>(triangle.a.position.x + 0.5f);
	int32_t x2 = static_cast<int32_t>(triangle.b.position.x + 0.5f);
	int32_t x3 = static_cast<int32_t>(triangle.c.position.x + 0.5f);
	int32_t y1 = static_cast<int32_t>(triangle.a.position.y + 0.5f);
	int32_t y2 = static_cast<int32_t>(triangle.b.position.y + 0.5f);
	int32_t y3 = static_cast<int32_t>(triangle.c.position.y + 0.5f);

	float z1_inv = 1.0f / triangle.a.position.z;
	float z2_inv = 1.0f / triangle.b.position.z;
	float z3_inv = 1.0f / triangle.c.position.z;

	float u1 = triangle.a.uv.x * z1_inv;
	float u2 = triangle.b.uv.x * z2_inv;
	float u3 = triangle.c.uv.x * z3_inv;
	float v1 = triangle.a.uv.y * z1_inv;
	float v2 = triangle.b.uv.y * z2_inv;
	float v3 = triangle.c.uv.y * z3_inv;

	// Sort by Y axis
	if (y1 > y2) { swap(y1, y2); swap(x1, x2); swap(u1, u2); swap(v1, v2); swap(z1_inv, z2_inv); }
	if (y1 > y3) { swap(y1, y3); swap(x1, x3); swap(u1, u3); swap(v1, v3); swap(z1_inv, z3_inv); }
	if (y2 > y3) { swap(y2, y3); swap(x2, x3); swap(u2, u3); swap(v2, v3); swap(z2_inv, z3_inv); }

	int dy1 = y2 - y1, dx1 = x2 - x1;
	int dy2 = y3 - y1, dx2 = x3 - x1;
	float du1 = u2 - u1, dv1 = v2 - v1, dz1 = z2_inv - z1_inv;
	float du2 = u3 - u1, dv2 = v3 - v1, dz2 = z3_inv - z1_inv;

	float dax_step = (dy1 != 0) ? static_cast<float>(dx1) / abs(dy1) : 0;
	float dbx_step = (dy2 != 0) ? static_cast<float>(dx2) / abs(dy2) : 0;
	float du1_step = (dy1 != 0) ? du1 / abs(dy1) : 0;
	float dv1_step = (dy1 != 0) ? dv1 / abs(dy1) : 0;
	float dz1_step = (dy1 != 0) ? dz1 / abs(dy1) : 0;
	float du2_step = (dy2 != 0) ? du2 / abs(dy2) : 0;
	float dv2_step = (dy2 != 0) ? dv2 / abs(dy2) : 0;
	float dz2_step = (dy2 != 0) ? dz2 / abs(dy2) : 0;

	auto drawScanline = [&](int y, int ax, int bx, float su, float eu, float sv, float ev, float sz, float ez) noexcept -> void
		{
			if (ax > bx)
			{
				swap(ax, bx);
				swap(su, eu);
				swap(sv, ev);
				swap(sz, ez);
			}

			const int width = bx - ax;
			if (width == 0)
			{
				return;
			}

			const float tstep = 1.0f / width;
			float t = 0.0f;

			for (int x = ax; x < bx; ++x, t += tstep)
			{
				const float u = su + t * (eu - su);
				const float v = sv + t * (ev - sv);
				const float z = sz + t * (ez - sz);

				drawPixel((uint16_t)x, (uint16_t)y, u / z, v / z, 1.0f / z);
			}
		};

	// Upper part
	for (int y = y1; y <= y2; ++y)
	{
		const int ax = x1 + static_cast<int>((y - y1) * dax_step);
		const int bx = x1 + static_cast<int>((y - y1) * dbx_step);
		const float su = u1 + (y - y1) * du1_step;
		const float eu = u1 + (y - y1) * du2_step;
		const float sv = v1 + (y - y1) * dv1_step;
		const float ev = v1 + (y - y1) * dv2_step;
		const float sz = z1_inv + (y - y1) * dz1_step;
		const float ez = z1_inv + (y - y1) * dz2_step;

		drawScanline(y, ax, bx, su, eu, sv, ev, sz, ez);
	}

	dy1 = y3 - y2, dx1 = x3 - x2;
	du1 = u3 - u2, dv1 = v3 - v2, dz1 = z3_inv - z2_inv;

	dax_step = (dy1 != 0) ? static_cast<float>(dx1) / abs(dy1) : 0;
	du1_step = (dy1 != 0) ? du1 / abs(dy1) : 0;
	dv1_step = (dy1 != 0) ? dv1 / abs(dy1) : 0;
	dz1_step = (dy1 != 0) ? dz1 / abs(dy1) : 0;

	// Lower part
	for (int y = y2; y <= y3; ++y)
	{
		const int ax = x2 + static_cast<int>((y - y2) * dax_step);
		const int bx = x1 + static_cast<int>((y - y1) * dbx_step);
		const float su = u2 + (y - y2) * du1_step;
		const float eu = u1 + (y - y1) * du2_step;
		const float sv = v2 + (y - y2) * dv1_step;
		const float ev = v1 + (y - y1) * dv2_step;
		const float sz = z2_inv + (y - y2) * dz1_step;
		const float ez = z1_inv + (y - y1) * dz2_step;

		drawScanline(y, ax, bx, su, eu, sv, ev, sz, ez);
	}
}

#endif
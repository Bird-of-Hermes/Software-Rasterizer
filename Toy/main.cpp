#include "../Renderer/Renderer.hpp"

// may use a function as a shader
inline static Color myPixelShader(uint16_t x, uint16_t y) noexcept
{
	return Color{ (uint8_t)x, (uint8_t)y, 255 };
}

class MyEngine final : public Application
{
public:
	void OnInit() override
	{
		logResult(object.LoadFromFile("../bird-orange/BirdOrange.fbx"));

		object.scale = {9.f, 9.f, 9.f};
		object.positionInSpace.z = 10.0f;
		object.positionInSpace.y = 0.0f;
		object.positionInSpace.x = 0.0f;

		camera.position = { 0.0f, 35.0f, -40.0f };
		camera.projection.fieldOfView = 90.0f;

		// may also set a target to follow
		// pass the pointer to bind, call it with nullptr or with no arguments to unbind
		//camera.SetTarget(&object.positionInSpace);
	}

private:
	void OnUpdate([[maybe_unused]] float dt) noexcept override
	{
		SetWindowTitle("Birdie, FPS: " + std::to_string(FPS()));

		//// use this syntax for calling function members
		// const auto lambda = [this](uint16_t x, uint16_t y) -> Color { return pixelShader(x, y); };
		// DrawPixelShader(lambda);
		//// for default funcionts you just may pass the function to the PS, beware of name collisions though
		//DrawPixelShader(myPixelShader);




		// Use arrow keys for camera rotation and WASD for movement, CTRL and SHIFT are also used to travel upwards and downwards

		const float step = 30.0f * dt;
		if (Platform::Window::KeyDown(VK_SHIFT))
		{
			camera.position.y += step; // Travel Downwards
		}

		if (Platform::Window::KeyDown(VK_CONTROL))
		{
			camera.position.y -= step; // Travel Upwards
		}

		if (Platform::Window::KeyDown(VK_LEFT))
		{
			camera.rotation.y += 3.0f * step;
		}

		if (Platform::Window::KeyDown(VK_RIGHT))
		{
			camera.rotation.y -= 3.0f * step;
		}

		if (Platform::Window::KeyDown(VK_UP))
		{
			camera.rotation.x += 3.0f * step;
		}

		if (Platform::Window::KeyDown(VK_DOWN))
		{
			camera.rotation.x -= 3.0f * step;
		}

		const Vec3f forward = camera.forward() * step;
		const Vec3f left = camera.left() * step;

		if (Platform::Window::KeyDown('W'))
		{
			camera.position += forward;
		}
		if (Platform::Window::KeyDown('S'))
		{
			camera.position -= forward;
		}
		if (Platform::Window::KeyDown('A'))
		{
			camera.position += left;
		}
		if (Platform::Window::KeyDown('D'))
		{
			camera.position -= left;
		}

		// reset rotations
		if (Platform::Window::KeyDown('R'))
		{
			camera.rotation.x = 0.0f;
			camera.rotation.y = 0.0f;
			camera.rotation.z = 0.0f;
		}
		camera.UpdateViewMatrix();
		Draw3DObject(object, camera);
	}

	// may also use a member function as a shader, but must bind it in a lambda
	inline Color pixelShader(uint16_t x, uint16_t y) noexcept
	{
		return img.pixel(x, y);
	}

private:
	Object3D<Vertex> object;
	Image img;
	Camera camera;
};

int main()
{
	MyEngine engine;

	// change MBs to be preallocated for large models, always check console log for errors
	logResult(engine.Start(1280, 768, L"", MB(15)));

	return 0;
}
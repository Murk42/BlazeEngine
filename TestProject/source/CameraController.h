#pragma once

class CameraController :	
	public EventHandler<Input::Events::MouseScroll>,
	public EventHandler<Input::Events::MouseMotion>,
	public EventHandler<Input::Events::InputPostUpdateEvent>
{
public:
	CameraController();
	~CameraController();

	void SetWindow(Window* window);

	Mat4f GetViewMatrix() const;	
private:
	Window* window;

	Vec2f angles;
	Quatf rotation;
	Vec3f position;
	float speed;
	float dt;
	Stopwatch dtStopwatch;
	
	void OnEvent(Input::Events::MouseScroll event) override;
	void OnEvent(Input::Events::MouseMotion event) override;
	void OnEvent(Input::Events::InputPostUpdateEvent event) override;
};
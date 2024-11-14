#pragma once

class CameraController :	
	public EventHandler<Input::Events::MouseScroll>,
	public EventHandler<Input::Events::MouseMotion>,
	public EventHandler<Input::Events::InputPostUpdateEvent>
{
public:
	CameraController();
	~CameraController();

	void SetWindow(WindowBase* window);

	Mat4f GetViewMatrix() const;	
private:
	WindowBase* window;

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
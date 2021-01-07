#include "BlazeEngine/BlazeEngineMain.h"
using namespace Blaze;

class BaseFrame
{
public:
	struct Type
	{
		void (*construct)(BaseFrame*);
		void (*destruct)(BaseFrame*);
		size_t size;
	};
	inline static std::vector<Type> types;
	inline static std::vector<BaseFrame*> frames;
	inline static BaseFrame* selected = nullptr;	

	Console::Frame frame;

	BaseFrame(const Vec2i& pos, const Vec2i& size) : frame(pos, size) { }

	virtual void Startup() {}
	virtual void Update(char) {}
	virtual void Cleanup() {}
	virtual void Selected(BaseFrame*) {}
	virtual void Deselected(BaseFrame*) {}

	static void Select(BaseFrame* frame)
	{
		if (selected != nullptr)
			selected->Deselected(frame);
		std::swap(selected, frame);
		if (selected != nullptr)
			selected->Selected(frame);
	}
};

template<typename T>
class Frame : public BaseFrame
{
	static void Construct(BaseFrame* f) { new (f) T(); }
	static void Destruct(BaseFrame* f) { new (f) T(); }
	struct Init
	{		
		Init()
		{
			BaseFrame::types.push_back({ Construct, Destruct, sizeof(T) });
		}		
	} inline static init;
public:
	Frame(const Vec2i& pos, const Vec2i& size) : BaseFrame(pos, size) { }
};

class App : public Application::Base<App>
{
public:
	void Startup() override 
	{				
		for (auto& t : BaseFrame::types)
		{
			BaseFrame* frame = (BaseFrame*)new uint8[t.size];
			t.construct(frame);
			BaseFrame::frames.push_back(frame);
		}
		for (auto& f : BaseFrame::frames)
			f->Startup();
	}

	void Cleanup() override 
	{  
		for (auto& f : BaseFrame::frames)
			f->Cleanup();
		for (uint i = 0; i < BaseFrame::types.size(); ++i)		
		{
			BaseFrame::types[i].destruct(BaseFrame::frames[i]);
			delete[] BaseFrame::frames[i];
		}
	}  

	void Frame() override 
	{	 
		char input = Console::GetKeyPressed();
		for (auto& f : BaseFrame::frames)
			f->Update(input);
	} 
}; 

class ActionMenu : public Frame<ActionMenu>
{
public:
	ActionMenu() : Frame({ 0, 0 }, { 40, 1 }) { }

	StringView actions[2] { "Load", "Save" };
	static constexpr uint actionCount = _countof(actions);
	uint action = 0;

	void Update(char input) override
	{		
		switch (input)
		{
		case 'a':
			Select(this);
			break;
		case (char)Console::Key::Left:
			if (action == 0)
				action = actionCount - 1;
			else
				--action;
			break;
		case (char)Console::Key::Right:
			action++;
			if (action == actionCount)
				action = 0; 
			break;
		case (char)Console::Key::Enter:
			DoAction();
			break;
		}		

		frame.Clear();		

		frame.MoveCursor({ 0, 0 });
		for (uint i = 0; i < actionCount; ++i)
		{
			if (action == i && selected == this)
				frame.Write(F_BLUE);			

			frame.Write(actions[i]);
			frame.Write(F_DEFAULT "|");			
		}
		
		frame.Refresh();
	}

	void Selected(BaseFrame*)
	{
		action = 0;


	}

	void DoAction()
	{
		//switch ()
		{

		}
	}
};

class ResourceMenu : public Frame<ResourceMenu>
{
public:
	ResourceMenu() : Frame({0, 3}, {30, 20}) { }

	void Update(char input) override
	{
		frame.Clear();


		frame.Refresh();
	}
};
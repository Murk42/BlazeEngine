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

struct SceneData
{
	String path;
};

class App : public Application::Base<App>
{
public:
	std::vector<SceneData> scenes;

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

	void Load(const StringView& path)
	{
		ByteStream bs;
		File::Read(path, &bs);		

		if (bs.Get<char>() == 'b')
		{
			scenes.resize(bs.Get<uint32>());
			sceneCount = bs.Get<uint32>();
			sceneFileData = new SceneFileData[sceneCount];
			for (uint i = 0; i < sceneCount; ++i)
			{
				sceneFileData[i].type = BaseScene::sceneTypes.data() + bs.Get<uint32>();

				sceneFileData[i].path.Resize(bs.Get<uint32>());
				bs.Get(sceneFileData[i].path.Ptr(), sceneFileData[i].path.Size());

				sceneFileData[i].name.Resize(bs.Get<uint32>());
				bs.Get(sceneFileData[i].name.Ptr(), sceneFileData[i].name.Size());
			}
		}
		else
		{
			StringStream ss = std::move(bs);
			sceneCount = ss.Get<uint32>();
			sceneFileData = new SceneFileData[sceneCount];
			for (uint i = 0; i < sceneCount; ++i)
			{
				sceneFileData[i].type = BaseScene::sceneTypes.data() + ss.Get<uint32>();
				if (ss.Get<char>() != '\"') return;
				sceneFileData[i].path = ss.GetUntil('\"');
				ss.Skip(1);
				if (ss.Get<char>() != '\"') return;
				sceneFileData[i].name = ss.GetUntil('\"');
				sceneFileData[i].type;
			}
		}
	}

	void Save(const StringView& path)
	{
		ByteStream bs;


		File::Write(path, true, &bs);
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
		switch (action)
		{
		case 0:
			break;
		case 1:
			break;
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
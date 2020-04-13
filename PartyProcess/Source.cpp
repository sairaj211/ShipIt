#include <Gear.h>



class Source : public Engine
{
public:
	Source() : Engine("MainMenu", true)
	{
 	}

	~Source()
	{

	}
};

Engine* CreateApplication()
{
	return new Source;
}
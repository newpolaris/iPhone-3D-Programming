#include "Interfaces.hpp"

using namespace std;

class ResourceManager : public IResourceManager {
public:
	string GetResourcePath() const
	{
		// NSString* bundlePath = [[NSBundle mainBundle] resourcePath];
		// return [bundlePath UTF8String];
		return "Models/";
	}
};

IResourceManager* CreateResourceManager()
{
	return new ResourceManager();
}
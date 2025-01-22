
#include <btBulletCollisionCommon.h>
#include <iostream> 

// Custom callback class
class MyContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    // Override the addSingleResult method
    btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override {
        // Process collision results here
        std::cout << "Collision detected!" << std::endl;
        std::cout.flush();
        return 0; // Return the applied impulse or any custom value if needed
    }
};
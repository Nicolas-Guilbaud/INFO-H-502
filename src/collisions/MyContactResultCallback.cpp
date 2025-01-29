
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
      
        // Get collision point in world coordinates
        btVector3 collisionPoint = cp.getPositionWorldOnA();  // or cp.getPositionWorldOnB()
        
        // Print collision details
        std::cout << "\rCollision detected at position (" << collisionPoint.x() << ", "
            << collisionPoint.y() << ", "
            << collisionPoint.z() << ")" << std::flush;
        
        return 0; // Return the applied impulse or any custom value if needed
    }
};
#include"IEnemy.h"

IEnemy::IEnemy() {

}
IEnemy::~IEnemy() {

}

Vector3 IEnemy::CreateVelocity(Vector3 translate){

    Vector3 velocity = {0.0f,0.0f,0.0f};
    if (translate.x == 56) {
        velocity.x = -1;
    }
    if (translate.x == -56) {
        velocity.x = 1;
    }
    if (translate.y == 52) {
        velocity.y = -1;
    }
    if (translate.y == -52) {
        velocity.y = 1;
    }
    velocity.x = std::clamp(velocity.x, -1.0f, 1.0f);
    velocity.y = std::clamp(velocity.y, -1.0f, 1.0f);
    velocity.z = std::clamp(velocity.z, -1.0f, 1.0f);

    return velocity;
}




 

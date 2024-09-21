#include"IEnemy.h"

IEnemy::IEnemy() {
    texManager_ = Texturemanager::GetInstance();

    reticleTextureHandle_ = texManager_->Load("resource/white.png");

    reticleWorld_.Initialize();

    reticleWorld_.scale_.x = 2.0f;
    reticleWorld_.scale_.y = 2.0f;
    reticleWorld_.translation_ = { 0.0f,0.0f,-1.0f };

    reticleWorld_.UpdateMatrix();

    reticleModel_.reset(new Plane);
    reticleModel_->Initialize();

}

IEnemy::~IEnemy() {

}
void IEnemy::CommonUpdate()
{
    if (isReticleUse) {

        reticleWorld_.UpdateMatrix();
    }

}

void IEnemy::CommonDraw(const ViewProjection& viewProjection)
{
    if (isReticleUse) {
    reticleModel_->Draw(reticleWorld_, viewProjection, {0.0f,1.0f,0.25f,0.4f}, reticleTextureHandle_);
    }
}

void IEnemy::setReticle(Vector3 transform)
{
    reticleWorld_.translation_ = transform;
    reticleWorld_.translation_.z = 1.0f;

    if (velocity_.x != 0.0f) {
        reticleWorld_.scale_.x = 500.0f;
    }
    if (velocity_.y != 0.0f) {
        reticleWorld_.scale_.y = 500.0f;
    }
    reticleWorld_.UpdateMatrix();
}

Vector3 IEnemy::CreateVelocity(Vector3 translate){

    Vector3 velocity = {0.0f,0.0f,0.0f};
    if (translate.x >= 56) {
        velocity.x = -1;
    }
    if (translate.x <= -56) {
        velocity.x = 1;
    }
    if (translate.y >= 52) {
        velocity.y = -1;
    }
    if (translate.y <= -52) {
        velocity.y = 1;
    }
    velocity.x = std::clamp(velocity.x, -1.0f, 1.0f);
    velocity.y = std::clamp(velocity.y, -1.0f, 1.0f);
    velocity.z = std::clamp(velocity.z, -1.0f, 1.0f);

    return velocity;
}




 

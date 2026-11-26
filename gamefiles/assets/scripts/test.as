const float PI = 3.14159265f;

class EntityTest{
    float yaw = 0;
    float pitch = 0;

    void OnStart() {
    }

    void OnUpdate(float deltaTime) {
        float speed = 5.0f * deltaTime;
        float sens  = 0.15f;

        vec2 mousedelta = MouseGetDelta();
        yaw   -= mousedelta.x * sens;
        pitch -= mousedelta.y * sens;
        if(pitch >  89.0f) pitch =  89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
        TransformSetRotation(pitch, yaw, 0);

        float rad = yaw * PI / 180.0f;
        float fx = -sin(rad);  float fz = -cos(rad);
        float rx =  cos(rad);  float rz = -sin(rad);

        if(KeyboardIsPressed(Keyboard::W)) TransformMove( fx*speed, 0,  fz*speed);
        if(KeyboardIsPressed(Keyboard::S)) TransformMove(-fx*speed, 0, -fz*speed);
        if(KeyboardIsPressed(Keyboard::D)) TransformMove( rx*speed, 0,  rz*speed);
        if(KeyboardIsPressed(Keyboard::A)) TransformMove(-rx*speed, 0, -rz*speed);

        if(KeyboardIsPressed(Keyboard::SPACE))  TransformMove(0,  speed, 0);
        if(KeyboardIsPressed(Keyboard::LSHIFT)) TransformMove(0, -speed, 0);
    }

    void OnDestroy() {
    }
}
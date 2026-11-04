class EntityTest{
    void OnStart() {

    }

    void OnUpdate(float deltaTime) {
        float velocity = 5.0 * deltaTime;


        if(KeyboardIsPressed(Keyboard::A)){
            TransformMove(-velocity,0,0);
        }
        if(KeyboardIsPressed(Keyboard::D)){
            TransformMove(velocity,0,0);
        }
        if(KeyboardIsPressed(Keyboard::W)){
            TransformMove(0,0,velocity);
        }
        if(KeyboardIsPressed(Keyboard::S)){
            TransformMove(0,0,-velocity);
        }
        if(KeyboardIsPressed(Keyboard::SPACE)){
            TransformMove(0,-velocity,0);
        }
        if(KeyboardIsPressed(Keyboard::LSHIFT)){
            TransformMove(0,velocity,0);
        }
        if(KeyboardIsPressed(Keyboard::LEFT)){
            TransformRotate(0,1,0);
        }
        if(KeyboardIsPressed(Keyboard::RIGHT)){
            TransformRotate(0,-1,0);
        }
        vec3 pos = TransformGetRotation();

        //LogEngine("x: " + formatFloat(pos.x,"",2)+" y: " + formatFloat(pos.y,"",2)+" z: " + formatFloat(pos.z,"",2));
    }

    void OnDestroy(){
        
    }
}
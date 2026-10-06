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

        vec3 pos = TransformGetPosition();

        //LogEngine("x: " + formatFloat(pos.x,"",2)+" y: " + formatFloat(pos.y,"",2)+" z: " + formatFloat(pos.z,"",2));
    }

    void OnDestroy(){
        
    }
}
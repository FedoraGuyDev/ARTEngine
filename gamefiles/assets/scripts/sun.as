class Sun{
    void OnStart() {
    }

    void OnUpdate(float deltaTime) {
        if(KeyboardIsPressed(Keyboard::F)){
            TransformRotate(1,0,0);
        }
        if(KeyboardIsPressed(Keyboard::G)){
            TransformRotate(-1,0,0);
        }
    }

    void OnDestroy(){
        
    }
}
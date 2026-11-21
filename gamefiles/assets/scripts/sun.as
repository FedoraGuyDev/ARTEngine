class Sun{
    void OnStart() {
    }

    void OnUpdate(float deltaTime) {
        if(KeyboardIsPressed(Keyboard::F)){
            TransformRotate(0,1,0);
        }
        if(KeyboardIsPressed(Keyboard::G)){
            TransformRotate(0,-1,0);
        }
    }

    void OnDestroy(){
        
    }
}
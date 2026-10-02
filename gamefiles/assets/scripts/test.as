class EntityTest{
    void OnStart() {

    }

    void OnUpdate(float deltaTime) {
        //LogEngine(formatInt(Keyboard::A));
        if(KeyboardIsJustPressed(Keyboard::A)){
            LogEngine("Hola Mundo");
        }
    }

    void OnDestroy(){
        
    }
}
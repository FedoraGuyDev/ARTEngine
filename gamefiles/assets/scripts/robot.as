class Robot{
    float timer = 0;

    void OnStart() {
        PlayAnimation("Robot_Idle", true, 0.0);
    }

    void OnUpdate(float deltaTime) {
        ///Teclas 1 a 4 cambian la animacion con un fundido de 0.3 segundos
        if(KeyboardIsJustPressed(Keyboard::N1)) PlayAnimation("Robot_Idle", true, 0.3);
        if(KeyboardIsJustPressed(Keyboard::N2)) PlayAnimation("Robot_Walking", true, 0.3);
        if(KeyboardIsJustPressed(Keyboard::N3)) PlayAnimation("Robot_Running", true, 0.3);
        if(KeyboardIsJustPressed(Keyboard::N4)) PlayAnimation("Robot_Punch", false, 0.1);

        ///Cuando termina el golpe (sin loop) vuelve a Idle
        if(IsAnimationFinished()) PlayAnimation("Robot_Idle", true, 0.3);
    }

    void OnDestroy(){
    }
}

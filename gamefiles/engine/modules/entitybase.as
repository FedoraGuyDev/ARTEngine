shared class EntityBase {
    float x = 0;
    float y = 0;
    float z = 0;
    float culling_radius = 0;
    bool is_persistant = false;

    void init() {}
    void update(float dt) {}
    void draw() {}
    void draw_screen() {}
}
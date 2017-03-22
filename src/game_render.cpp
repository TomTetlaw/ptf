#include "precompiled.h"
#include "game.h"

void immediate_render_line(Vector2 a, Vector2 b, Vector4 colour) {
    glColor4f(colour.x, colour.y, colour.z, colour.w);
    
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void immediate_render_box(Box box, Vector4 colour, bool fill) {
    if(fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glColor4f(colour.x, colour.y, colour.z, colour.w);

    glBegin(GL_QUADS);
    glVertex2f(box.x, box.y);
    glVertex2f(box.x, box.y + box.height);
    glVertex2f(box.x + box.width, box.y + box.height);
    glVertex2f(box.x + box.width, box.y);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    if(fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}
#include "precompiled.h"
#include "game.h"

void load_texture(const char *filename, Texture *texture) {
    Load_Texture_Result result = imports->load_texture_from_file(filename);

	unsigned int t;
	glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
	glTextureParameteriEXT(t, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteriEXT(t, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteriEXT(t, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteriEXT(t, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, result.pixels);
	glObjectLabel(GL_TEXTURE, t, -1, filename);

    texture->width = result.width;
    texture->height = result.height;
    texture->api_object = t;
}

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

void immediate_render_texture(Texture *texture, Vector2 position) {
    glBindTexture(GL_TEXTURE_2D, texture->api_object);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(position.x, position.y);
    glTexCoord2f(0, 1);
    glVertex2f(position.x, position.y + texture->height);
    glTexCoord2f(1, 1);
    glVertex2f(position.x + texture->width, position.y + texture->height);
    glTexCoord2f(1, 0);
    glVertex2f(position.x + texture->width, position.y);
    glEnd();
}
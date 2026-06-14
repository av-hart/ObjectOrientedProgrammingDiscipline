#ifndef LIB_H
#define LIB_H

#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <memory>
#include <cmath>

using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "gdiplus.lib")

// ================= БАЗОВИЙ КЛАС =================
class GraphicObject {
public:
    float x, y;
    float vx, vy;
    float w, h;
    Color color;

    GraphicObject(float x, float y, float w, float h, float vx, float vy, Color c)
        : x(x), y(y), w(w), h(h), vx(vx), vy(vy), color(c) {}

    virtual void draw(Graphics& g) = 0;

    virtual void update(int W, int H) {
        x += vx;
        y += vy;

        // відскок від стін (контур)
        if (x < 0 || x + w > W) vx = -vx;
        if (y < 0 || y + h > H) vy = -vy;

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + w > W) x = W - w;
        if (y + h > H) y = H - h;
    }

    // КОНТУРНЕ ЗІТКНЕННЯ (AABB)
    bool collide(GraphicObject* o) {
        return !(x + w < o->x || o->x + o->w < x ||
                 y + h < o->y || o->y + o->h < y);
    }

    // ВІДШТОВХУВАННЯ + ЗМІНА КОЛЬОРУ
    void onCollision(GraphicObject* o) {

        // ===== зміна кольору =====
        color = Color(255,
            (color.GetR() + 80) % 255,
            (color.GetG() + 50) % 255,
            (color.GetB() + 120) % 255
        );

        o->color = Color(255,
            (o->color.GetR() + 90) % 255,
            (o->color.GetG() + 60) % 255,
            (o->color.GetB() + 100) % 255
        );

        // ===== центри =====
        float cx1 = x + w / 2;
        float cy1 = y + h / 2;

        float cx2 = o->x + o->w / 2;
        float cy2 = o->y + o->h / 2;

        float dx = cx1 - cx2;
        float dy = cy1 - cy2;

        float absX = fabs(dx);
        float absY = fabs(dy);

        // ===== ВІДШТОВХУВАННЯ ОБ'ЄКТІВ МІЖ СОБОЮ =====
        if (absX > absY) {

            // удар зліва/справа → як вертикальна стіна
            vx = -vx;

            // розштовхуємо
            float push = (w + o->w) / 2 - absX;

            if (dx > 0) {
                x += push / 2;
                o->x -= push / 2;
            } else {
                x -= push / 2;
                o->x += push / 2;
            }
        }
        else {

            // удар зверху/знизу → як горизонтальна стіна
            vy = -vy;

            float push = (h + o->h) / 2 - absY;

            if (dy > 0) {
                y += push / 2;
                o->y -= push / 2;
            } else {
                y -= push / 2;
                o->y += push / 2;
            }
        }
    }

    virtual ~GraphicObject() {}
};

// ================= ПРЯМОКУТНИК =================
class RectangleObj : public GraphicObject {
public:
    RectangleObj(float x, float y, float w, float h, float vx, float vy)
        : GraphicObject(x, y, w, h, vx, vy, Color(255, 0, 0, 200)) {}

    void draw(Graphics& g) override {
        SolidBrush brush(color);
        g.FillRectangle(&brush, x, y, w, h);
    }
};

// ================= КОЛО =================
class CircleObj : public GraphicObject {
public:
    CircleObj(float x, float y, float r, float vx, float vy)
        : GraphicObject(x, y, r * 2, r * 2, vx, vy, Color(255, 0, 200, 0)) {}

    void draw(Graphics& g) override {
        SolidBrush brush(color);
        g.FillEllipse(&brush, x, y, w, h);
    }
};

// ================= ТРИКУТНИК =================
class TriangleObj : public GraphicObject {
public:
    TriangleObj(float x, float y, float size, float vx, float vy)
        : GraphicObject(x, y, size, size, vx, vy, Color(255, 200, 100, 50)) {}

    void draw(Graphics& g) override {

        PointF p1(x + w / 2, y);
        PointF p2(x, y + h);
        PointF p3(x + w, y + h);

        PointF points[3] = { p1, p2, p3 };

        SolidBrush brush(color);
        g.FillPolygon(&brush, points, 3);
    }
};

#endif
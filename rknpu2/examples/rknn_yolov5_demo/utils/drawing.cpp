#include "drawing.h"
#include "string.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

void draw_rectangle_c1(unsigned char* pixels, int w, int h, int stride, int rx, int ry, int rw, int rh, unsigned int color, int thickness)
{
    const unsigned char* pen_color = (const unsigned char*)&color;

    if (thickness == -1)
    {
        // filled
        for (int y = ry; y < ry + rh; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx; x < rx + rw; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x] = pen_color[0];
            }
        }

        return;
    }

    const int t0 = thickness / 2;
    const int t1 = thickness - t0;

    // draw top
    {
        for (int y = ry - t0; y < ry + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x] = pen_color[0];
            }
        }
    }

    // draw bottom
    {
        for (int y = ry + rh - t0; y < ry + rh + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x] = pen_color[0];
            }
        }
    }

    // draw left
    for (int x = rx - t0; x < rx + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x] = pen_color[0];
        }
    }

    // draw right
    for (int x = rx + rw - t0; x < rx + rw + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x] = pen_color[0];
        }
    }
}

void draw_rectangle_c2(unsigned char* pixels, int w, int h, int stride, int rx, int ry, int rw, int rh, unsigned int color, int thickness)
{
    const unsigned char* pen_color = (const unsigned char*)&color;

    if (thickness == -1)
    {
        // filled
        for (int y = ry; y < ry + rh; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx; x < rx + rw; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 2 + 0] = pen_color[0];
                p[x * 2 + 1] = pen_color[1];
            }
        }

        return;
    }

    const int t0 = thickness / 2;
    const int t1 = thickness - t0;

    // draw top
    {
        for (int y = ry - t0; y < ry + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 2 + 0] = pen_color[0];
                p[x * 2 + 1] = pen_color[1];
            }
        }
    }

    // draw bottom
    {
        for (int y = ry + rh - t0; y < ry + rh + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 2 + 0] = pen_color[0];
                p[x * 2 + 1] = pen_color[1];
            }
        }
    }

    // draw left
    for (int x = rx - t0; x < rx + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 2 + 0] = pen_color[0];
            p[x * 2 + 1] = pen_color[1];
        }
    }

    // draw right
    for (int x = rx + rw - t0; x < rx + rw + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 2 + 0] = pen_color[0];
            p[x * 2 + 1] = pen_color[1];
        }
    }
}

void draw_rectangle_c3(unsigned char* pixels, int w, int h, int stride, int rx, int ry, int rw, int rh, unsigned int color, int thickness)
{
    const unsigned char* pen_color = (const unsigned char*)&color;

    if (thickness == -1)
    {
        // filled
        for (int y = ry; y < ry + rh; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx; x < rx + rw; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 3 + 0] = pen_color[0];
                p[x * 3 + 1] = pen_color[1];
                p[x * 3 + 2] = pen_color[2];
            }
        }

        return;
    }

    const int t0 = thickness / 2;
    const int t1 = thickness - t0;

    // draw top
    {
        for (int y = ry - t0; y < ry + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 3 + 0] = pen_color[0];
                p[x * 3 + 1] = pen_color[1];
                p[x * 3 + 2] = pen_color[2];
            }
        }
    }

    // draw bottom
    {
        for (int y = ry + rh - t0; y < ry + rh + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 3 + 0] = pen_color[0];
                p[x * 3 + 1] = pen_color[1];
                p[x * 3 + 2] = pen_color[2];
            }
        }
    }

    // draw left
    for (int x = rx - t0; x < rx + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 3 + 0] = pen_color[0];
            p[x * 3 + 1] = pen_color[1];
            p[x * 3 + 2] = pen_color[2];
        }
    }

    // draw right
    for (int x = rx + rw - t0; x < rx + rw + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 3 + 0] = pen_color[0];
            p[x * 3 + 1] = pen_color[1];
            p[x * 3 + 2] = pen_color[2];
        }
    }
}

void draw_rectangle_c4(unsigned char* pixels, int w, int h, int stride, int rx, int ry, int rw, int rh, unsigned int color, int thickness)
{
    const unsigned char* pen_color = (const unsigned char*)&color;

    if (thickness == -1)
    {
        // filled
        for (int y = ry; y < ry + rh; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx; x < rx + rw; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 4 + 0] = pen_color[0];
                p[x * 4 + 1] = pen_color[1];
                p[x * 4 + 2] = pen_color[2];
                p[x * 4 + 3] = pen_color[3];
            }
        }

        return;
    }

    const int t0 = thickness / 2;
    const int t1 = thickness - t0;

    // draw top
    {
        for (int y = ry - t0; y < ry + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 4 + 0] = pen_color[0];
                p[x * 4 + 1] = pen_color[1];
                p[x * 4 + 2] = pen_color[2];
                p[x * 4 + 3] = pen_color[3];
            }
        }
    }

    // draw bottom
    {
        for (int y = ry + rh - t0; y < ry + rh + t1; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            for (int x = rx - t0; x < rx + rw + t1; x++)
            {
                if (x < 0)
                    continue;

                if (x >= w)
                    break;

                p[x * 4 + 0] = pen_color[0];
                p[x * 4 + 1] = pen_color[1];
                p[x * 4 + 2] = pen_color[2];
                p[x * 4 + 3] = pen_color[3];
            }
        }
    }

    // draw left
    for (int x = rx - t0; x < rx + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 4 + 0] = pen_color[0];
            p[x * 4 + 1] = pen_color[1];
            p[x * 4 + 2] = pen_color[2];
            p[x * 4 + 3] = pen_color[3];
        }
    }

    // draw right
    for (int x = rx + rw - t0; x < rx + rw + t1; x++)
    {
        if (x < 0)
            continue;

        if (x >= w)
            break;

        for (int y = ry + t1; y < ry + rh - t0; y++)
        {
            if (y < 0)
                continue;

            if (y >= h)
                break;

            unsigned char* p = pixels + stride * y;

            p[x * 4 + 0] = pen_color[0];
            p[x * 4 + 1] = pen_color[1];
            p[x * 4 + 2] = pen_color[2];
            p[x * 4 + 3] = pen_color[3];
        }
    }
}

void draw_rectangle_yuv420sp(unsigned char* yuv420sp, int w, int h, int rx, int ry, int rw, int rh, unsigned int color, int thickness)
{
    // assert w % 2 == 0
    // assert h % 2 == 0
    // assert rx % 2 == 0
    // assert ry % 2 == 0
    // assert rw % 2 == 0
    // assert rh % 2 == 0
    // assert thickness % 2 == 0

    const unsigned char* pen_color = (const unsigned char*)&color;

    unsigned int v_y;
    unsigned int v_uv;
    unsigned char* pen_color_y = (unsigned char*)&v_y;
    unsigned char* pen_color_uv = (unsigned char*)&v_uv;
    pen_color_y[0] = pen_color[0];
    pen_color_uv[0] = pen_color[1];
    pen_color_uv[1] = pen_color[2];

    unsigned char* Y = yuv420sp;
    draw_rectangle_c1(Y, w, h, w, rx, ry, rw, rh, v_y, thickness);

    unsigned char* UV = yuv420sp + w * h;
    int thickness_uv = thickness == -1 ? thickness : max(thickness / 2, 1);
    draw_rectangle_c2(UV, w / 2, h / 2, w, rx / 2, ry / 2, rw / 2, rh / 2, v_uv, thickness_uv);
}


void draw_image_yuv420sp(unsigned char* yuv420sp, int w, int h, unsigned char* draw_img, int rx, int ry, int rw, int rh) {
    for (int i = 0; i < rh; i++) {
        memcpy(yuv420sp + (ry+i) * w + rx,  draw_img + i * rw,  rw);
    }
    for (int i = 0; i < rh/2; i++) {
        memcpy(yuv420sp + w*h + (ry/2+i) * w+ rx, draw_img + rw*rh + i*rw, rw);
    }
}
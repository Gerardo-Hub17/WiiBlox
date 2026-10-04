#include <gccore.h>
#include "renderer.h"

void Renderer_Init(void)
{
}

void Renderer_Draw(void)
{

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    GX_Position3f32(-0.6f, -0.6f, -2.0f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32(0.6f, -0.6f, -2.0f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32(0.6f, 0.6f, -2.0f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32(-0.6f, 0.6f, -2.0f);
    GX_Color4u8(255, 0, 0, 255);

    GX_End();

}

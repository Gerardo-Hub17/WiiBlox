#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

static void *xfb[2];
static GXRModeObj *rmode;
static u32 xfb_index = 0;

static void *fifo = NULL;

static void init_video(void)
{
    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    xfb[0] = MEM_K0_TO_K1(
        SYS_AllocateFramebuffer(rmode)
    );

    xfb[1] = MEM_K0_TO_K1(
        SYS_AllocateFramebuffer(rmode)
    );

    VIDEO_Configure(rmode);

    VIDEO_SetNextFramebuffer(xfb[xfb_index]);

    VIDEO_SetBlack(FALSE);

    VIDEO_Flush();
    VIDEO_WaitVSync();

    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
}

static void init_gx(void)
{
    fifo = memalign(32, 256 * 1024);

    GX_Init(fifo, 256 * 1024);

    GX_SetViewport(
        0,
        0,
        rmode->fbWidth,
        rmode->efbHeight,
        0,
        1
    );

    GX_SetScissor(
        0,
        0,
        rmode->fbWidth,
        rmode->efbHeight
    );

    GX_SetDispCopySrc(
        0,
        0,
        rmode->fbWidth,
        rmode->efbHeight
    );

    GX_SetDispCopyDst(
        rmode->fbWidth,
        rmode->xfbHeight
    );

    GX_SetCopyFilter(
        rmode->aa,
        rmode->sample_pattern,
        GX_TRUE,
        rmode->vfilter
    );

    GX_SetPixelFmt(
        GX_PF_RGB8_Z24,
        GX_ZC_LINEAR
    );

    GX_SetCullMode(GX_CULL_NONE);
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);

    GX_SetNumTexGens(0);
    GX_SetNumTevStages(1);

    GX_SetTevOp(
        GX_TEVSTAGE0,
        GX_PASSCLR
    );

    GX_SetVtxDesc(
        GX_VA_POS,
        GX_DIRECT
    );

    GX_SetVtxDesc(
        GX_VA_CLR0,
        GX_DIRECT
    );

    GX_SetVtxAttrFmt(
        GX_VTXFMT0,
        GX_VA_POS,
        GX_POS_XYZ,
        GX_F32,
        0
    );

    GX_SetVtxAttrFmt(
        GX_VTXFMT0,
        GX_VA_CLR0,
        GX_CLR_RGBA,
        GX_RGBA8,
        0
    );

    GX_InvVtxCache();
}

static void draw_cube(void)
{
    /*
     * Cube centered at the origin.
     *
     * Coordinates:
     *
     *        (-,+,+) -------- (+,+,+)
     *          /|               /|
     *         / |              / |
     *        /  |             /  |
     * (-,-,+) -------- (+,-,+)  |
     *       |   |             |  |
     *       | (-,+,-) --------|--(+,+,-)
     *       |  /              | /
     *       | /               |/
     *       |/                /
     * (-,-,-) -------- (+,-,-)
     */

    GX_Begin(
        GX_QUADS,
        GX_VTXFMT0,
        24
    );

    /*
     * Front face
     * Red
     */
    GX_Position3f32(-0.5f, -0.5f,  0.5f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32( 0.5f, -0.5f,  0.5f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32( 0.5f,  0.5f,  0.5f);
    GX_Color4u8(255, 0, 0, 255);

    GX_Position3f32(-0.5f,  0.5f,  0.5f);
    GX_Color4u8(255, 0, 0, 255);

    /*
     * Back face
     * Blue
     */
    GX_Position3f32( 0.5f, -0.5f, -0.5f);
    GX_Color4u8(0, 0, 255, 255);

    GX_Position3f32(-0.5f, -0.5f, -0.5f);
    GX_Color4u8(0, 0, 255, 255);

    GX_Position3f32(-0.5f,  0.5f, -0.5f);
    GX_Color4u8(0, 0, 255, 255);

    GX_Position3f32( 0.5f,  0.5f, -0.5f);
    GX_Color4u8(0, 0, 255, 255);

    /*
     * Left face
     * Green
     */
    GX_Position3f32(-0.5f, -0.5f, -0.5f);
    GX_Color4u8(0, 255, 0, 255);

    GX_Position3f32(-0.5f, -0.5f,  0.5f);
    GX_Color4u8(0, 255, 0, 255);

    GX_Position3f32(-0.5f,  0.5f,  0.5f);
    GX_Color4u8(0, 255, 0, 255);

    GX_Position3f32(-0.5f,  0.5f, -0.5f);
    GX_Color4u8(0, 255, 0, 255);

    /*
     * Right face
     * Yellow
     */
    GX_Position3f32(0.5f, -0.5f,  0.5f);
    GX_Color4u8(255, 255, 0, 255);

    GX_Position3f32(0.5f, -0.5f, -0.5f);
    GX_Color4u8(255, 255, 0, 255);

    GX_Position3f32(0.5f,  0.5f, -0.5f);
    GX_Color4u8(255, 255, 0, 255);

    GX_Position3f32(0.5f,  0.5f,  0.5f);
    GX_Color4u8(255, 255, 0, 255);

    /*
     * Top face
     * White
     */
    GX_Position3f32(-0.5f, 0.5f,  0.5f);
    GX_Color4u8(255, 255, 255, 255);

    GX_Position3f32( 0.5f, 0.5f,  0.5f);
    GX_Color4u8(255, 255, 255, 255);

    GX_Position3f32( 0.5f, 0.5f, -0.5f);
    GX_Color4u8(255, 255, 255, 255);

    GX_Position3f32(-0.5f, 0.5f, -0.5f);
    GX_Color4u8(255, 255, 255, 255);

    /*
     * Bottom face
     * Magenta
     */
    GX_Position3f32(-0.5f, -0.5f, -0.5f);
    GX_Color4u8(255, 0, 255, 255);

    GX_Position3f32( 0.5f, -0.5f, -0.5f);
    GX_Color4u8(255, 0, 255, 255);

    GX_Position3f32( 0.5f, -0.5f,  0.5f);
    GX_Color4u8(255, 0, 255, 255);

    GX_Position3f32(-0.5f, -0.5f,  0.5f);
    GX_Color4u8(255, 0, 255, 255);

    GX_End();
}

static void draw_frame(void)
{
    Mtx44 projection;

    guPerspective(
        projection,
        60.0f,
        (f32)rmode->fbWidth / (f32)rmode->efbHeight,
        0.1f,
        100.0f
    );

    GX_LoadProjectionMtx(
        projection,
        GX_PERSPECTIVE
    );

    GX_SetViewport(
        0,
        0,
        rmode->fbWidth,
        rmode->efbHeight,
        0,
        1
    );

    GX_SetZMode(
        GX_TRUE,
        GX_LEQUAL,
        GX_TRUE
    );

    GX_SetTevOp(
        GX_TEVSTAGE0,
        GX_PASSCLR
    );

    GX_ClearVtxDesc();

    GX_SetVtxDesc(
        GX_VA_POS,
        GX_DIRECT
    );

    GX_SetVtxDesc(
        GX_VA_CLR0,
        GX_DIRECT
    );

    GX_SetVtxAttrFmt(
        GX_VTXFMT0,
        GX_VA_POS,
        GX_POS_XYZ,
        GX_F32,
        0
    );

    GX_SetVtxAttrFmt(
        GX_VTXFMT0,
        GX_VA_CLR0,
        GX_CLR_RGBA,
        GX_RGBA8,
        0
    );

    GX_SetCullMode(GX_CULL_NONE);

    GX_SetZMode(
        GX_TRUE,
        GX_LEQUAL,
        GX_TRUE
    );

    GX_SetCopyClear(
        (GXColor){ 20, 40, 80, 255 },
        0xFFFFFF
    );

    GX_SetScissor(
        0,
        0,
        rmode->fbWidth,
        rmode->efbHeight
    );

    draw_cube();

    GX_DrawDone();

    GX_SetZMode(
        GX_FALSE,
        GX_LEQUAL,
        GX_FALSE
    );

    GX_CopyDisp(
        xfb[xfb_index],
        GX_TRUE
    );

    GX_DrawDone();

    VIDEO_SetNextFramebuffer(
        xfb[xfb_index]
    );

    VIDEO_Flush();
}

int main(int argc, char **argv)
{
    init_video();
    init_gx();

    while (1)
    {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_HOME)
            break;

        draw_frame();

        xfb_index ^= 1;

        VIDEO_WaitVSync();
    }

    return 0;
}

#include "camera.h"
#include "../graphics/ThemeTextures.h"
#include "figure.h"

extern bool showdialogbox;
extern bool dboxInFrame;
extern bool dboxStopped;
extern bool dbox_showIcon;
extern bool dbox_selectMenu;

typedef enum
{
    FR_AGE_UNKNOWN= 0,
    FR_AGE_CHILD= 1,
    FR_AGE_TEEN= 2,
    FR_AGE_ADULT= 3,
} fr_age_t;

fr_age_t age_recognition()
{
    if (!dsiFeatures()) //Cannot check on the NDS, consider yourself lucky
        return FR_AGE_ADULT;

    clearText();
    updateText(false);

    printLarge(false, 0, 40, "We need to verify your age", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 70, "You need to be a verified adult to", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 80, "open this app.", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 100, "Please center your face on the", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 110, "screen to match the figure.", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 130, "Your privacy is important to us,", Alignment::center, FontPalette::dialog);
    printSmall(false, 0, 140, "your data will be deleted after this.", Alignment::center, FontPalette::dialog);

	showdialogbox = true;
    updateText(false);

    bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    u16* camera_fb_ptr= (u16*)malloc(256*256*sizeof(u16));
    int camera_fb_x= 24;
    int camera_fb_y= 24;
    int camera_fb_w= 208;
    int camera_fb_h= 156;

    // for (int iy=camera_fb_y; iy<camera_fb_y+camera_fb_h; iy++)
    // {
    //     for (int ix=camera_fb_x; ix<camera_fb_x+camera_fb_w; ix++)
    //         camera_fb_ptr[ix+iy*256]= 0x8000+ix+256*iy;
    // }

    cameraInit();
    cameraActivate(CAM_INNER);

    uint16_t kp= 0;
    fr_age_t result= FR_AGE_UNKNOWN;

	while (1)
    {
        scanKeys();
        kp= keysDown();

        if (kp&KEY_B)
            result= FR_AGE_UNKNOWN;
        if (kp&KEY_A)
            result= FR_AGE_ADULT;
        if (kp&KEY_X)
            result= FR_AGE_TEEN;
        if (kp&KEY_Y)
            result= FR_AGE_CHILD;

        if (kp&(KEY_A|KEY_B|KEY_X|KEY_Y))
            break;

        cameraTransferStart(camera_fb_ptr, CAPTURE_MODE_PREVIEW);

        //Draw camera buffer
        for (int iy=camera_fb_y; iy<camera_fb_y+camera_fb_h; iy++)
        {
            for (int ix=camera_fb_x; ix<camera_fb_x+camera_fb_w; ix++)
                ((u16*)0x06200000)[ix+iy*256]= camera_fb_ptr[ix+iy*256];
        }
        //Draw figure on top
        for (int iy=0; iy<64; iy++)
        {
            for (int ix=0; ix<64; ix++)
            {
                if (figureBitmap[ix+iy*64])
                    ((u16*)0x06200000)[ix+(camera_fb_x+(camera_fb_w>>1)-32)+(iy+camera_fb_y+(camera_fb_h>>1)-32)*256]= 0xFFFF;
            }
        }

        while(cameraTransferActive())
            swiWaitForVBlank();
	}

	cameraTransferStop();
	cameraDeactivate(CAM_INNER);
    free(camera_fb_ptr);
    tex().commitBgSubModify();//drawOverBoxArt(camera_fb_w, camera_fb_h);
    //memset(camera_fb_ptr, 0x0000, 256*256);
    showdialogbox= false;
    return result;
}

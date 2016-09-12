
#include "dxfw.h"

using namespace base;


static const CanvasLayerDesc _ldesc[] = {
    {0,"o*=saturate(1-length(wpos)/8);",0,0},
    {0,"killmask(t,255,0,255,10); o=mask(t,0,0,255,10)?c:t; o*=saturate(1-length(wpos)/8);",0,0},
    {}
};


DevCanvas can(_ldesc,"data");
DevCanvas ui;

static const CanvasLayerDesc _cfont[] = {
	{0,"",RSF_BLEND_ALPHA,TEXF_MIPMAP},
    {}
};
DevCanvas cfont(_cfont);

DevTileSet font("data/antik2pl",32,4,
				" !\"Æ£%Ñ'()*+,-./0123456789:;<=>?\n"
				"ŒABCDEFGHIJKLMNOPQRSTUVWXYZ¯\\^_\n"
				"`¹```ê``````³`ñó```œ````Ÿ`¿\n"
				"`abcdefghijklmnopqrstuvwxyz");

DevTileSet tiles("data/druid_tiles",11,6," |-1234\n````````$\n````*");

DevTxFont txf("data/Arial_Black_27");




void Render()
{
	vec2 ss = Dev.GetScreenSizeV();

	Dev->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,0x008000,1,0);


	static const char mp[] = {
		"1----2\n"
		"| * $|\n"
		"|   *|\n"
		"3----4\n"
	};

	can.SetView(vec2(2,3),8);
	tiles.Draw(can,0,vec2(0,0),1,mp);
    can.Draw(1,"druid")(fmod((float)Dev.GetElapsedTime(),.3f)<.15f ? 1 : 0)(0x800080)(vec2(1.5f,1.5f),.3f);
    can.Draw(1,"druid")()(0x0000FF).linerep(vec2(0,3),vec2(2,3),.2f);

    for(int i=0;i<Dev.GetMiceCount();i++)
    {
        RawMouse &m = Dev.GetMiceData(i);
		m.ClampPosV(vec2(-.25f,-.25f)/0.01f,vec2(6.25f,4.25f)/0.01f);
        can.Draw(1,"druid")()((i ? 0x7F007F : 0x007F7F)*(m.bt_down ? 2 : 1))(m.GetPos()*.01f,.3f);
	    
        font.Draw(ui,0,vec2(0,3+float(i)),1,format("%d %d %d %d",m.bt_down,m.dz,int(ss.x),int(ss.y)).c_str());
    }
    
    can.Flush();


	ui.SetView(vec2(40/2,30/2),30);
	font.Draw(ui,0,vec2(0,0),1,format("Ala ma kota!\nScore: %d",int(Dev.GetElapsedTime()*100)).c_str());
	ui.Flush();


	cfont.SetView(ss*.5f,ss.y);
	txf.DrawTextF(cfont,0,100,100,0x11,.75f,-1,"Hello World!");

	cfont.Flush();
}



int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR pCmdLine,int nShowCmd)
{
//	Config cfg("config.cfg");
//	Dev.SetResolution(cfg.GetInt("screen_w",800),cfg.GetInt("screen_h",600),false);

	Dev.SetConfig("device.cfg");
	Dev.SetResolution(800,600,false);

	while(Dev.MainLoop())
    {
     //   Dev.SetMouseCapture(true);
		Render();
    }

	return 0;
}

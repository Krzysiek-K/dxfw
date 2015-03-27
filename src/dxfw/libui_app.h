
#ifndef _DXFW_LIBUI_APP_H
#define _DXFW_LIBUI_APP_H


class UIWindowFactory;
class UIDeviceD3D;
class UISystem;


class DxFwGUIApp : public IDevMsgHandler {
public:
	DxFwGUIApp(const char *app_name,UIWindowFactory *wfactory,const char *skin) : 
		ui_dev(NULL), ui_sys(NULL)
	{
		Dev.RegisterMessageHandler(this);

		last_mouse_shape = -1;

		IDirect3DDevice9 *dev = Dev.GetDevice();
		if(!dev) return;

		int sx, sy;
		Dev.GetScreenSize(sx,sy);

		ui_dev = new UIDeviceD3D(dev,skin ? skin : "*");
		ui_dev->UpdateScreenSize(ui_coord_t(sx,sy));

		ui_sys = new UISystem(ui_dev,wfactory,skin ? skin : "*");
		ui_sys->UpdateScreenSize(ui_coord_t(sx,sy));

		UpdateMouseState(true);
	}

	~DxFwGUIApp()
	{
		Shutdown();
	}

	void Shutdown()
	{
		Dev.UnregisterMessageHandler(this);
		if(ui_dev) { delete ui_dev; ui_dev = NULL; }
		if(ui_sys) { delete ui_sys; ui_sys = NULL; }
	}


	void OnRender(float delta,bool is_active)
	{
		if(!ui_sys) return;
		if(!is_active) OnDeactivate();
		ui_sys->Render(delta);
		UpdateMouseState(false);
	}

	void OnDeactivate()
	{
		if(ui_sys) ui_sys->Deactivate();
	}

	UISystem *GetUISystem() { return ui_sys; }


private:
	UIDeviceD3D		*ui_dev;
	UISystem		*ui_sys;
	int				last_mouse_shape;



	void OnWindowResize(int w,int h)
	{
		if(ui_dev) ui_dev->UpdateScreenSize(ui_coord_t(w,h));
		if(ui_sys) ui_sys->UpdateScreenSize(ui_coord_t(w,h));
		UpdateMouseState(true);
	}

	void OnMouseDown(int mb,int mx,int my)
	{
		if(!ui_sys || !ui_dev) return;
		if(mb==1) ui_sys->UpdateKeyState(VK_LBUTTON,true);
		if(mb==2) ui_sys->UpdateKeyState(VK_RBUTTON,true);
		if(mb==4) ui_sys->UpdateKeyState(VK_MBUTTON,true);
		UpdateMouseState(true);
	}

	void OnMouseUp(int mb,int mx,int my)
	{
		if(!ui_sys || !ui_dev) return;
		if(mb==1) ui_sys->UpdateKeyState(VK_LBUTTON,false);
		if(mb==2) ui_sys->UpdateKeyState(VK_RBUTTON,false);
		if(mb==4) ui_sys->UpdateKeyState(VK_MBUTTON,false);
		UpdateMouseState(true);
	}

	void OnMouseMove(int mx,int my)
	{
		if(!ui_sys) return;
		ui_sys->UpdateMousePos( ui_coord_t(mx,my) );
		UpdateMouseState(true);
	}


	void OnKeyDown(int key)
	{
		if(!ui_sys) return;
		ui_sys->UpdateKeyState(key,true);
		UpdateMouseState(false);
	}

	void OnKeyUp(int key)
	{
		if(!ui_sys) return;
		ui_sys->UpdateKeyState(key,false);
		UpdateMouseState(false);
	}

	void OnCharTyped(int ch)
	{
		if(!ui_sys) return;
		ui_sys->SendTypedChar(ch);
		UpdateMouseState(false);
	}

	void UpdateMouseState(bool force_shape)
	{
		if(!ui_sys) return;

		int shape = ui_sys->GetMouseShape();
		if(force_shape || shape!=last_mouse_shape)
		{
			HCURSOR cursor = NULL;
			switch(shape)
			{
				case UISystem::SHAPE_NONE:		cursor = NULL;								break;
				case UISystem::SHAPE_ARROW:		cursor = LoadCursor(NULL, IDC_ARROW);		break;
				case UISystem::SHAPE_HORIZ:		cursor = LoadCursor(NULL, IDC_SIZEWE);		break;
				case UISystem::SHAPE_VERT:		cursor = LoadCursor(NULL, IDC_SIZENS);		break;
				case UISystem::SHAPE_BOTH:		cursor = LoadCursor(NULL, IDC_SIZEALL);		break;
				case UISystem::SHAPE_DRAG:		cursor = LoadCursor(NULL, IDC_APPSTARTING);	break;
				default:						cursor = LoadCursor(NULL, IDC_ARROW);		break;
			}
			SetCursor(cursor);
			last_mouse_shape = shape;
		}

		ui_coord_t mpos;
		if(ui_sys->GetMousePos(mpos))
		{
			POINT pt = { mpos.x, mpos.y };
			ClientToScreen(Dev.GetHWnd(),&pt);
			SetCursorPos(pt.x,pt.y);
		}
	}


public:
	virtual LRESULT MsgProc(Device *dev,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam,bool &pass_forward)
	{
		bool mouse_affected = false;
		int mouse_bt_down = 0;
		int mouse_bt_up = 0;

		switch(msg)
		{
//			case WM_MOVE:	OnWindowMove((short) LOWORD(lParam),(short) HIWORD(lParam));	break;
			case WM_SIZE:	OnWindowResize(LOWORD(lParam),HIWORD(lParam));					break;
			case WM_MOUSEMOVE:		mouse_affected=true;									break;
			case WM_LBUTTONDOWN:	mouse_affected=true;	mouse_bt_down=1;				break;
			case WM_RBUTTONDOWN:	mouse_affected=true;	mouse_bt_down=2;				break;
			case WM_MBUTTONDOWN:	mouse_affected=true;	mouse_bt_down=4;				break;
			case WM_LBUTTONUP:		mouse_affected=true;	mouse_bt_up=1;					break;
			case WM_RBUTTONUP:		mouse_affected=true;	mouse_bt_up=2;					break;
			case WM_MBUTTONUP:		mouse_affected=true;	mouse_bt_up=4;					break;

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:		OnKeyDown(wParam);										break;

			case WM_SYSKEYUP:
			case WM_KEYUP:			OnKeyUp(wParam);										break;

			case WM_CHAR:			OnCharTyped(wParam);									break;
			case WM_SYSCHAR:		OnCharTyped(wParam | 0x10000);							return TRUE;

		}

		if(mouse_affected)
		{
			int mx = LOWORD(lParam);
			int my = HIWORD(lParam);
//			if(mouse_bt_down)
//				activated = true;
				 if(mouse_bt_down)	OnMouseDown(mouse_bt_down,mx,my);
			else if(mouse_bt_up)	OnMouseUp(mouse_bt_up,mx,my);
			else					OnMouseMove(mx,my);
			pass_forward = false;
			return 0;
		}

		return 0;
	}

};




#endif

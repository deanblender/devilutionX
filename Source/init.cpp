#include "diablo.h"

#include <array>
#include <string>

#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>
#include <config.h>

DEVILUTION_BEGIN_NAMESPACE

_SNETVERSIONDATA fileinfo;
int gbActive;
static char *diablo_exe_path = nullptr;
HANDLE hellfire_mpq;
static char *patch_rt_mpq_path = nullptr;
WNDPROC CurrentProc;
HANDLE diabdat_mpq;
static char *diabdat_mpq_path = nullptr;
HANDLE patch_rt_mpq;

/* data */

char gszVersionNumber[MAX_PATH] = "internal version unknown";
char gszProductName[MAX_PATH] = "Diablo v1.09";

void init_cleanup(BOOL show_cursor)
{
	pfile_flush_W();

	if (diabdat_mpq) {
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = NULL;
	}
	if (patch_rt_mpq) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = NULL;
	}
	if (hellfire_mpq) {
		SFileCloseArchive(hellfire_mpq);
		hellfire_mpq = NULL;
	}

	UiDestroy();
	effects_cleanup_sfx();
	sound_cleanup();
	NetClose();
	dx_cleanup();
	engine_debug_trap(show_cursor);

	if (show_cursor)
		ShowCursor(TRUE);
}

void init_create_window(int nCmdShow)
{
	int nWidth, nHeight;
	HWND hWnd;

	if (GetSystemMetrics(SM_CXSCREEN) < SCREEN_WIDTH)
		nWidth = SCREEN_WIDTH;
	else
		nWidth = GetSystemMetrics(SM_CXSCREEN);
	if (GetSystemMetrics(SM_CYSCREEN) < SCREEN_HEIGHT)
		nHeight = SCREEN_HEIGHT;
	else
		nHeight = GetSystemMetrics(SM_CYSCREEN);
	hWnd = CreateWindowEx(0, "DIABLO", "DIABLO", WS_POPUP, 0, 0, nWidth, nHeight, NULL, NULL, ghInst, NULL);
	if (!hWnd)
		app_fatal("Unable to create main window");
	ShowWindow(hWnd, SW_SHOWNORMAL); // nCmdShow used only in beta: ShowWindow(hWnd, nCmdShow)
	UpdateWindow(hWnd);
	dx_init(hWnd);
	BlackPalette();
	snd_init(hWnd);
	init_archives();
	SDL_DisableScreenSaver();
}

void init_archives()
{
	HANDLE fh;
	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.size = sizeof(fileinfo);
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = diablo_exe_path;
	fileinfo.originalarchivefile = diabdat_mpq_path;
	fileinfo.patcharchivefile = patch_rt_mpq_path;
	init_get_file_info();
#ifdef SPAWN
		diabdat_mpq = init_test_access(&diabdat_mpq_path, "spawn.mpq", "DiabloSpawn", 1000, FS_PC);
#else
		diabdat_mpq = init_test_access(&diabdat_mpq_path, "diabdat.mpq", "DiabloCD", 1000, FS_PC);
#endif
	if (!WOpenFile("ui_art\\title.pcx", &fh, TRUE))
#ifdef SPAWN
		FileErrDlg("Main program archive: spawn.mpq");
#else
		FileErrDlg("Main program archive: diabdat.mpq");
#endif
	WCloseFile(fh);
#ifdef SPAWN
	patch_rt_mpq = init_test_access(&patch_rt_mpq_path, "patch_sh.mpq", "DiabloSpawn", 2000, FS_PC);
#else
	patch_rt_mpq = init_test_access(&patch_rt_mpq_path, "patch_rt.mpq", "DiabloInstall", 2000, FS_PC);
#endif
}

HANDLE init_test_access(char **mpq_path, char *mpq_name, char *reg_loc, int flags, int fs)
{
	std::array<std::string, 2> mpqPaths = {
		GetBasePath() + mpq_name,
		GetPrefPath() + mpq_name,
	};

	for (const std::string &path : mpqPaths) {
		HANDLE archive;
		if (SFileOpenArchive(path.c_str(), flags, MPQ_FLAG_READ_ONLY, &archive)) {
			*mpq_path = new char[path.size() + 1];
			strcpy(*mpq_path, path.c_str());
			return archive;
		}
	}

	return NULL;
}

void init_get_file_info()
{
	snprintf(gszProductName, MAX_PATH, "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	snprintf(gszVersionNumber, MAX_PATH, "version %s", PROJECT_VERSION);
}

LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case WM_ERASEBKGND:
		return 0;
	case WM_CREATE:
		ghMainWnd = hWnd;
		break;
	case WM_DESTROY:
		init_cleanup(1);
		ghMainWnd = 0;
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		drawpanflag = 255;
		break;
	case WM_CLOSE:
		return 0;
	case WM_ACTIVATEAPP:
		init_activate_window(hWnd, wParam);
		break;
#ifdef _DEBUG
	case WM_SYSKEYUP:
		if (wParam == VK_RETURN) {
			fullscreen = !fullscreen;
			dx_reinit();
			return 0;
		}
		break;
#endif
	case WM_QUERYNEWPALETTE:
		return 1;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void init_activate_window(HWND hWnd, BOOL bActive)
{
	gbActive = bActive;
	UiAppActivate(bActive);

	if (gbActive) {
		drawpanflag = 255;
		ResetPal();
	}
}

WNDPROC SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc;

	OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

DEVILUTION_END_NAMESPACE

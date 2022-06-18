/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const int gaps               = 1;        /* 0 means no gaps */
static const int gappx              = 25;       /* gaps between windows */
static const unsigned int snap      = 20;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#1f1f1f";
static const char col_gray2[]       = "#282A2E";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_green[]       = "#20ff30";
static const char col_black[]       = "#000000";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_green, col_gray1, col_black },
	[SchemeSel]  = { col_gray1, col_green,  col_green  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	/* { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 }, */
        { "ark",      NULL,       NULL,       0,            1,           -1 },
        { "kdeconnect", NULL,     NULL,       0,            1,           -1 },
        /* { "ksysguard",  NULL,     NULL,       0,            1,           -1 }, */
        { "kcalc",    NULL,       NULL,       0,            1,           -1 },
        { NULL, "pulseaudio-equalizer-gtk", NULL, 0,        1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const float sfact     = 0.25; /* factor of second area size [0.05..0.95] */
static const int symmetry    = 1;    /* use symmetrical setup instead of sfact */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
        { "[3]",      thirdtile},
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* custom function declarations */
static void pulseaudioctl(const Arg *arg);
static void backlight(const Arg *arg);
static void exec(const Arg *arg);
static void suspend(const Arg *arg);
static void night(const Arg *arg);
static void nextwallpaper(const Arg *arg);
static void bluetooth(const Arg *arg);
static void displayoff(const Arg *arg);
static void togglesym(const Arg *arg);
static void togglegaps(const Arg *arg);
static void setgaps(const Arg *arg);

/* night mode */
static int nightmode = 0;
/* https://askubuntu.com/questions/1003101/how-to-use-xrandr-gamma-for-gnome-night-light-like-usage */
static const char *nightcol[][3] = {
        {"1",           "1",            "1"},
        {"1",           "0.97107439",   "0.94305985"},
        {"1",           "0.93853986",   "0.88130458"},
        {"1",           "0.90198230",   "0.81465502"},
        {"1",           "0.86860704",   "0.73688797"},
        {"1",           "0.82854786",   "0.64816570"},
        {"1",           "0.77987699",   "0.54642268"},
        {"1",           "0.71976951",   "0.42860152"},
};

/* wallpapers */
static const char wallpapers[] = "~/Pictures/more/wallpapers/Minimalistic-Wallpaper-Collection/images";

/* bluetooth device to optionally connect to */
static const char btdevice[] = "00:13:EF:A0:08:DC";

/* displays to optionally turn off */
static const char optdisplay[] = "eDP";
static const char optdisplay2[] = "DisplayPort-0";

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_gray3, "-sb", col_green, "-sf", col_black, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setsfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_l,      setsfact,       {.f = +0.05} },
        { MODKEY,                       XK_s,      togglesym,      {0} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_g,      setgaps,        {.i = +5 } },
	{ MODKEY|ControlMask,           XK_g,      setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_g,      togglegaps,     {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
        { Mod4Mask,                     XK_s,      suspend,        {0} },
        { Mod4Mask,                     XK_o,      displayoff,     {0} },
        { Mod4Mask|ShiftMask,		XK_o,      displayoff,     {1} },
        { Mod4Mask,                     XK_e,      exec,         {.v = "dolphin"} },
        { Mod4Mask,                     XK_q,      exec,         {.v = "qutebrowser"} },
        { Mod4Mask,                     XK_Up,     pulseaudioctl,  {1} },
        { Mod4Mask,                     XK_k,      pulseaudioctl,  {1} },
        { Mod4Mask,                     XK_Down,   pulseaudioctl,  {0} },
        { Mod4Mask,                     XK_j,      pulseaudioctl,  {0} },
        { Mod4Mask,                     XK_space,  pulseaudioctl,  {3} },
        { Mod4Mask,                     XK_m,      pulseaudioctl,  {2} },
        { Mod4Mask,                     XK_d,      bluetooth,      {0} },
        { Mod4Mask,                     XK_Left,   backlight,      {0} },
        { Mod4Mask,                     XK_h,      backlight,      {0} },
        { Mod4Mask,                     XK_Right,  backlight,      {1} },
        { Mod4Mask,                     XK_l,      backlight,      {1} },
        { Mod4Mask,                     XK_n,      night,          {0} },
        { Mod4Mask|ShiftMask,           XK_n,      night,          {1} },
        { Mod4Mask,                     XK_w,      nextwallpaper,  {0} },
};

/* button definitions */
/* click can be ClkClientWin or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
};

/* custom functions */
void
pulseaudioctl(const Arg *arg) {
    switch(arg->i)
    {
        case 0:
            system("pulseaudio-control down");
        break;
        case 1:
            system("pulseaudio-control --volume-max 100 up");
        break;
        case 2:
            system("pulseaudio-control togmute");
        break;
        default:
            system("pulseaudio-control next-sink");
    }
}

void
backlight(const Arg *arg) {
    int direction = arg->i;
    int currentvalue;
    // TODO read current value from actualbrightness file
    // TODO write new value into brightness file
}

void
exec(const Arg *arg) {
    if(fork() == 0) {
        system(arg->v);
        exit(0);
    }
}

void
suspend(const Arg *arg) {
    system("systemctl suspend; slock");
}

void
night(const Arg *arg) {
    int len = sizeof nightcol / sizeof nightcol[0];
    if(arg->i == 0) {
        nightmode++;
        if(nightmode == len)
        {
            nightmode--;
            return;
        }
    }
    else {
        nightmode--;
        if(nightmode < 0)
        {
            nightmode = 0;
            return;
        }
    }

    char rcmd[128], gcmd[128], bcmd[128];
    strcpy(rcmd, "xgamma -rgamma ");
    strcpy(gcmd, "xgamma -ggamma ");
    strcpy(bcmd, "xgamma -bgamma ");
    system(strcat(rcmd, nightcol[nightmode][0]));
    system(strcat(gcmd, nightcol[nightmode][1]));
    system(strcat(bcmd, nightcol[nightmode][2]));
}

void
nextwallpaper(const Arg *arg) {
    char cmd[1024];
    strcpy(cmd, "feh --bg-fill --randomize ");
    system(strcat(cmd, wallpapers));
}

void
bluetooth(const Arg *arg) {
    if(fork() == 0) {
        char cmd[64];
        strcpy(cmd, "bluetoothctl power on && bluetoothctl connect ");
        system(strcat(cmd, btdevice));
        exit(0);
    }
}

void
displayoff(const Arg *arg) {
    char cmd[128];
    strcpy(cmd, "xrandr --output ");

    if (arg->i == 0)
	    strcat(cmd, optdisplay);
    else
	    strcat(cmd, optdisplay2);

    system(strcat(cmd, " --off"));
}

void
togglesym(const Arg *arg)
{
	selmon->symmetry = !selmon->symmetry;
	arrange(selmon);
}

void
togglegaps(const Arg *arg)
{
	selmon->gaps = !selmon->gaps;
	arrange(selmon);
}

void
setgaps(const Arg *arg)
{
	if (selmon->gappx + arg->i <= 0)
                return;

	selmon->gappx += arg->i;
	arrange(selmon);
}

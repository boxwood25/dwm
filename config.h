/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#1f1f1f";
static const char col_gray2[]       = "#333333";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_green[]       = "#20ff30";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_green, col_gray1, col_gray2 },
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
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* custom function declarations */
static void pulseaudioctl(const Arg *arg);
static void backlight(const Arg *arg);
static void dolphin(const Arg *arg);
static void suspend(const Arg *arg);
static void night(const Arg *arg);

/* night mode */
static int nightmode = 0;
static const float nightcol[][3] = {
        {1.0,       1.0,        1.0},
        // TODO
};

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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_green, "-sf", col_gray1, NULL };
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
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
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
        { Mod4Mask,                     XK_e,      dolphin,        {0} },
        { Mod4Mask,                     XK_Up,     pulseaudioctl,  {1} },
        { Mod4Mask,                     XK_k,      pulseaudioctl,  {1} },
        { Mod4Mask,                     XK_Down,   pulseaudioctl,  {0} },
        { Mod4Mask,                     XK_j,      pulseaudioctl,  {0} },
        { Mod4Mask,                     XK_space,  pulseaudioctl,  {3} },
        { Mod4Mask,                     XK_m,      pulseaudioctl,  {2} },
        { Mod4Mask,                     XK_Left,   backlight,      {0} },
        { Mod4Mask,                     XK_h,      backlight,      {0} },
        { Mod4Mask,                     XK_Right,  backlight,      {1} },
        { Mod4Mask,                     XK_l,      backlight,      {1} },
        { Mod4Mask,                     XK_n,      night,          {0} },
        { Mod4Mask|ShiftMask,           XK_n,      night,          {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

/* custom functions */
void
pulseaudioctl(const Arg *arg) {
    int status;

    switch(arg->i)
    {
        case 0:
            status = system("pulseaudio-control down");
        break;
        case 1:
            status = system("pulseaudio-control --volume-max 100 up");
        break;
        case 2:
            status = system("pulseaudio-control togmute");
        break;
        default:
            status = system("pulseaudio-control next-sink");
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
dolphin(const Arg *arg) {
    if(fork() == 0) {
        int status = system("dolphin");
        exit(0);
    }
}

void
suspend(const Arg *arg) {
    system("systemctl suspend && slock");
}

void
night(const Arg *arg) {
    int len = sizeof nightcol / sizeof nightcol[0];
    if(arg->i == 0) {
        nightmode++;
        if(nightmode == len)
            nightmode--;
    }
    else {
        nightmode--;
        if(nightmode < 0)
            nightmode = 0;
    }

    int max = 50;
    char r[max], g[max], b[max];
    //TODO create the string values of the floats

    system("xgamma -rgamma " + nightcol[nightmode][0]);
    system("xgamma -ggamma " + nightcol[nightmode][1]);
    system("xgamma -bgamma " + nightcol[nightmode][2]);
}

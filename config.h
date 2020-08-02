/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const int user_bh            = 32;       /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const unsigned int gappih    = 8;        /* horiz inner gap between windows */
static const unsigned int gappiv    = 8;        /* vert inner gap between windows */
static const unsigned int gappoh    = 8;        /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 8;        /* vert outer gap between windows and screen edge */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "SF Mono:style=semibold:size=11",
                                        "mononoki Nerd Font Mono:size=14",
                                        "JoyPixels:pixelsize=16:antialias=true:autohint=true",
                                        "Unifont:pixelsize=14:antialias=true:autohint=true" };
static const char dmenufont[]       = "mononoki Nerd Font Mono:style=bold:size=12";
static const char scratchpadname[]  = "scratchpad";
static const char *scratchpadcmd[]  = { "st", "-t", scratchpadname, "-g", "120x34", "-e", "tmux", NULL };
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#ffffff";
static const char col_gray5[]       = "#37474F";
static const char col_cyan[]        = "#42A5F5";
static const char selfgcolor[]      = "#eeeeee";
static const char col_red[]         = "#770000";
static const char statusfg[]        = "#ebdbb2";
static const char tagnormbg[]       = "#282828";
static const char tagnormborder[]   = "#1d2021";
static const char tagselbg[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]  = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]   = { statusfg,  tagselbg,  col_cyan  },
        [SchemeTitle] = { col_gray3, col_gray1, col_cyan  },
	[SchemeUrg]   = { col_gray4, col_cyan,  "#ff0000" },
};

static const char *const autostart[] = {
        "sh", "-c", "dropbox start &", NULL,
        "sh", "-c", "xautolock -time 60 -locker blurlock &", NULL,
	"sh", "-c", "sleep 10s; nohup dwm_bar &", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */

	/* class,         instance, title,                    tags mask, isfloating, isterminal, noswallow, monitor */
	{ "Gimp",         NULL,     NULL,                     0,         1,          0,          0,         -1},
	{ "Firefox",      NULL,     NULL,                     1 << 8,    0,          0,          0,         -1},
	{ "Chromium",     NULL,     NULL,                     1 << 2,    0,          0,          1,         -1},
	{ "St",           NULL,     NULL,                     1 << 1,    0,          1,          0,         -1},
	{ "Pcmanfm",      NULL,     NULL,                     1 << 0,    0,          0,          0,         -1},
	{ "Pcmanfm",      NULL,     "Execute File",           1 << 0,    1,          0,          0,         -1},
	{ "Pcmanfm",      NULL,     "Confirm File Replacing", 1 << 0,    1,          0,          0,         -1},
	{ "Pavucontrol",  NULL,     NULL,                     1 << 0,    1,          0,          0,         -1},
	{ "Font-manager", NULL,     NULL,                     1 << 3,    0,          0,          0,         -1},
	{ "URxvt",        NULL,     NULL,                     0,         1,          1,          0,         -1},
	{ NULL,           NULL,     "Event Tester",           0,         0,          0,          1,         -1},
};

/* layout(s) */
static const float mfact     = 0.618; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
 	{ "[@]",      spiral },
 	{ "[\\]",     dwindle },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
/* static const char *dmenucmd[] = { "dmenu_run"}; */
static const char *termcmd[]  = { "st", NULL };

#include "movestack.c"
static Key keys[] = {
	/* modifier                key            function        argument */
	/* { MODKEY,                       XK_Return, zoom,           {0} }, */
	/* { MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} }, */
	/* { MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } }, */
	/* { MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } }, */
	/* { MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } }, */
	/* { MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } }, */
	/* { MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } }, */
	/* { MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } }, */

	{ 0,                        0,            quit,           {0} },
	{ 0,                        0,            zoom,           {0} },
	{ 0,                        0,            incrgaps,       {.i = +1 } },
	{ 0,                        0,            incrgaps,       {.i = -1 } },
	{ 0,                        0,            incrogaps,      {.i = +1 } },
	{ 0,                        0,            incrogaps,      {.i = -1 } },
	{ 0,                        0,            incrigaps,      {.i = +1 } },
	{ 0,                        0,            incrigaps,      {.i = -1 } },
	{ 0,                        0,            togglegaps,     {0} },
	{ 0,                        0,            defaultgaps,    {0} },
	{ 0,                        0,            incrihgaps,     {.i = +1 } },
	{ 0,                        0,            incrihgaps,     {.i = -1 } },
	{ 0,                        0,            incrivgaps,     {.i = +1 } },
	{ 0,                        0,            incrivgaps,     {.i = -1 } },
	{ 0,                        0,            incrohgaps,     {.i = +1 } },
	{ 0,                        0,            incrohgaps,     {.i = -1 } },
	{ 0,                        0,            incrovgaps,     {.i = +1 } },
	{ 0,                        0,            incrovgaps,     {.i = -1 } },

	{ MODKEY,                   XK_grave,     togglescratch,  {.v = scratchpadcmd } },
        { MODKEY,                   XK_Return,    spawn,          SHCMD("st") },
        { MODKEY,                   XK_z,         spawn,          {.v = dmenucmd } },
	{ MODKEY,                   XK_b,         togglebar,      {0} },
	{ MODKEY,                   XK_j,         focusstack,     {.i = +1 } },
	{ MODKEY,                   XK_k,         focusstack,     {.i = -1 } },
	{ MODKEY,                   XK_i,         incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_i,         incnmaster,     {.i = -1 } },
	{ MODKEY,                   XK_h,         setmfact,       {.f = -0.05} },
	{ MODKEY,                   XK_l,         setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,         XK_j,         movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_k,         movestack,      {.i = -1 } },
	{ MODKEY,                   XK_Tab,       view,           {0} },
	{ MODKEY,                   XK_q,         killclient,     {0} },
	{ MODKEY|Mod1Mask,          XK_1,         setlayout,      {.v = &layouts[0]} },        /* tile */
	{ MODKEY|Mod1Mask,          XK_2,         setlayout,      {.v = &layouts[1]} },        /* bstack */
	{ MODKEY|Mod1Mask,          XK_3,         setlayout,      {.v = &layouts[2]} },        /* spiral */
	{ MODKEY|Mod1Mask,          XK_4,         setlayout,      {.v = &layouts[3]} },        /* dwindle */
	{ MODKEY|Mod1Mask,          XK_5,         setlayout,      {.v = &layouts[4]} },        /* deck */
	{ MODKEY|Mod1Mask,          XK_6,         setlayout,      {.v = &layouts[5]} },        /* monocle */
	{ MODKEY|Mod1Mask,          XK_7,         setlayout,      {.v = &layouts[6]} },        /* centeredmaster */
	{ MODKEY|Mod1Mask,          XK_8,         setlayout,      {.v = &layouts[7]} },        /* centeredfloatingmaster */
	{ MODKEY,                   XK_Page_Down, cyclelayout,    {.i = -1 } },
	{ MODKEY,                   XK_Page_Up,   cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_space,     setlayout,      {0} },
	{ MODKEY,                   XK_space,     togglefloating, {0} },
	{ MODKEY,                   XK_f,         togglefullscr,  {0} },
	{ MODKEY,                   XK_0,         view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,         XK_0,         tag,            {.ui = ~0 } },
	{ MODKEY,                   XK_comma,     focusmon,       {.i = -1 } },
	{ MODKEY,                   XK_period,    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_comma,     tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_period,    tagmon,         {.i = +1 } },
	{ MODKEY,                   XK_F12,       spawn,          SHCMD("killall startdwm") },
	{ MODKEY|ShiftMask,         XK_equal,     setcfact,       {.f = +0.25} },
	{ MODKEY,                   XK_minus,     setcfact,       {.f = -0.25} },
	{ MODKEY,                   XK_equal,     setcfact,       {.f =  0.00} },
	{ MODKEY,                   XK_s,         togglesticky,   {0} },
	TAGKEYS(                    XK_1,         0)
	TAGKEYS(                    XK_2,         1)
	TAGKEYS(                    XK_3,         2)
	TAGKEYS(                    XK_4,         3)
	TAGKEYS(                    XK_5,         4)
	TAGKEYS(                    XK_6,         5)
	TAGKEYS(                    XK_7,         6)
	TAGKEYS(                    XK_8,         7)
	TAGKEYS(                    XK_9,         8)
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

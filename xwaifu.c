#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <Imlib2.h>

#define _NET_WM_STATE_ADD 1

Display *dpy;
int scr;
Window win;
int x, y;
unsigned w, h;
int img_w, img_h;

void
die(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

void
print_usage(void)
{
	fprintf(stderr, "usage: xwaifu [-fhrR] [-a ALPHA] [-g GEOMETRY] image_file\n\n");
	fprintf(stderr, "-a ALPHA    set image translucency\n");
	fprintf(stderr, "-f          hide image when hovered over\n");
	fprintf(stderr, "-g GEOMETRY set window position and/or size\n");
	fprintf(stderr, "-h          print this message\n");
	fprintf(stderr, "-r          set image width automatically\n");
	fprintf(stderr, "-R          set image height automatically\n");
}
	
void
change_wm_state(int action, char *state)
{
	XEvent event = {.type = ClientMessage};
	XClientMessageEvent *xclient = &event.xclient;

	xclient->window = win;
	xclient->message_type = XInternAtom(dpy, "_NET_WM_STATE", 0);
	xclient->format = 32;
	xclient->data.l[0] = action;
	xclient->data.l[1] = XInternAtom(dpy, state, 0);

	XSendEvent(dpy, DefaultRootWindow(dpy), 0,
			SubstructureNotifyMask | SubstructureRedirectMask, &event);
}

void
create_window(void)
{
	XVisualInfo vinfo;
	XMatchVisualInfo(dpy, scr, 32, TrueColor, &vinfo);
	Colormap colormap = XCreateColormap(dpy, DefaultRootWindow(dpy), vinfo.visual, AllocNone);
	XSetWindowAttributes attr = {.colormap = colormap, .border_pixel = 0, .background_pixel = 0};

	win = XCreateWindow(dpy, DefaultRootWindow(dpy), x, y, w, h, 0, vinfo.depth, InputOutput,
						vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);

	imlib_context_set_display(dpy);
	imlib_context_set_visual(vinfo.visual);
	imlib_context_set_drawable(win);
	imlib_context_set_colormap(colormap);

	Atom value = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", 0);
	XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", 0),
					XA_ATOM, 32, PropModeReplace,
					(unsigned char *) &value, 1);

	char *win_name = "xwaifu";
	XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_NAME", 0),
					XInternAtom(dpy, "UTF8_STRING", 0), 8, PropModeReplace,
					(unsigned char *) win_name, strlen(win_name));

	XClassHint class_hints = {.res_name = "xwaifu", .res_class = "xwaifu"};
	XSetClassHint(dpy, win, &class_hints);

	XSizeHints size_hints = {
		.x = x,
		.y = y,
		.width = w,
		.height = h,
		.min_width = w,
		.min_height = h,
		.max_width = w,
		.max_height = h,
		.flags = USPosition | USSize | PMinSize | PMaxSize
	};
	XSetWMNormalHints(dpy, win, &size_hints);

	XMapWindow(dpy, win);

	change_wm_state(_NET_WM_STATE_ADD, "_NET_WM_STATE_ABOVE");
	change_wm_state(_NET_WM_STATE_ADD, "_NET_WM_STATE_STICKY");
}

void
set_background(double alpha)
{
	Imlib_Image resized;
	if (!(resized = imlib_create_cropped_scaled_image(0, 0, img_w, img_h, w, h)))
		die("Couldn't allocate memory for image.");

	imlib_free_image_and_decache();
	imlib_context_set_image(resized);

	Pixmap background, mask;
	imlib_render_pixmaps_for_whole_image(&background, &mask);

	if (alpha != 1) {
		XFreePixmap(dpy, background);

		Imlib_Color_Modifier color_modifier;
		if (!(color_modifier = imlib_create_color_modifier()))
			die("Couldn't allocate memory for color modifier.");

		DATA8 rgb_arr[256], alpha_arr[256];
		for (int i = 0; i < 256; i++) {
			alpha_arr[i] = i * alpha;
			rgb_arr[i] = i;
		}

		imlib_context_set_color_modifier(color_modifier);
		imlib_set_color_modifier_tables(rgb_arr, rgb_arr, rgb_arr, alpha_arr);
		imlib_apply_color_modifier();
		imlib_free_color_modifier();

		/* Converts straight RGBA to premultiplied RGBA */
		Imlib_Image premul;
		if (!(premul = imlib_create_image(w, h)))
			die("Couldn't allocate memory for image.");
		imlib_context_set_image(premul);

		imlib_context_set_color(0, 0, 0, 255);
		imlib_image_fill_rectangle(0, 0, w, h);
		imlib_blend_image_onto_image(resized, 0, 0, 0, w, h, 0, 0, w, h);
		imlib_image_copy_alpha_to_image(resized, 0, 0);

		Pixmap dummy;
		imlib_render_pixmaps_for_whole_image(&background, &dummy);
		if (dummy)
			XFreePixmap(dpy, dummy);

		imlib_free_image_and_decache();
		imlib_context_set_image(resized);
	}

	imlib_free_image_and_decache();

	XShapeCombineMask(dpy, win, ShapeBounding, 0, 0, mask, ShapeSet);
	XSetWindowBackgroundPixmap(dpy, win, background);
	XClearWindow(dpy, win);

	XFreePixmap(dpy, background);
	if (mask)
		XFreePixmap(dpy, mask);
}

void
set_geometry(char *geometry, int auto_width, int auto_height)
{
	int geometry_bits = XParseGeometry(geometry, &x, &y, &w, &h);

	if (geometry_bits & XNegative)
		x = DisplayWidth(dpy, scr) - img_w + x;
	if (geometry_bits & YNegative)
		y = DisplayHeight(dpy, scr) - img_h + y;

	if ((geometry_bits & WidthValue) && (geometry_bits & HeightValue)) {
		if (auto_width)
			w = img_w * ((double) h / img_h);
		if (auto_height)
			h = img_h * ((double) w / img_w);
	} else {
		w = img_w;
		h = img_h;
	}
}

int
pointer_on_win_rect(void)
{
	Window dummy_win;
	int query_x, query_y, dummy_int;
	unsigned dummy_unsigned;
	XQueryPointer(dpy, win, &dummy_win, &dummy_win, &query_x, &query_y,
				  &dummy_int, &dummy_int, &dummy_unsigned);
	return query_x >= x && query_x <= x + w && query_y >= y && query_y <= y + h;
}

void
run(void)
{
	for (;;) {
		XEvent event;
		XNextEvent(dpy, &event);
		if (event.type == EnterNotify) {
			XUnmapWindow(dpy, win);
			while (pointer_on_win_rect())
				usleep(500 * 1000);
			XMapWindow(dpy, win);
		}
	}
}
	

int
main(int argc, char **argv)
{
	char *geometry;
	double alpha = 1;
	int auto_width = 0;
	int auto_height = 0;
	int fade = 0;

	int opt;
	while ((opt = getopt(argc, argv, "g:a:hrRf")) != -1) {
		switch (opt) {
		case 'g':
			geometry = optarg;
			break;
		case 'a':
			if (sscanf(optarg, "%lf", &alpha) != 1 || alpha < 0 || alpha > 1)
				die("Invalid alpha value.");
			break;
		case 'h':
			print_usage();
			exit(EXIT_SUCCESS);
			break;
		case 'r':
			auto_width = 1;
			break;
		case 'R':
			auto_height = 1;
			break;
		case 'f':
			fade = 1;
			break;
		}
	}

	if (optind > argc - 1) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	char *img_path = argv[optind];
	Imlib_Image img;
	if (!(img = imlib_load_image(img_path)))
		die("Couldn't allocate memory for image.");

	imlib_context_set_image(img);
	img_w = imlib_image_get_width();
	img_h = imlib_image_get_height();

	if (!(dpy = XOpenDisplay(NULL)))
		die("Couldn't open X display.");
	scr = DefaultScreen(dpy);

	set_geometry(geometry, auto_width, auto_height);
	if (w <= 0 || h <= 0)
		die("Invalid geometry.");

	create_window();
	if (fade) {
		XSelectInput(dpy, win, StructureNotifyMask | EnterWindowMask);
	} else {
		XSelectInput(dpy, win, NoEventMask);
		XShapeCombineRectangles(dpy, win, ShapeInput, 0, 0, NULL, 0, ShapeSet, Unsorted);
	}

	set_background(alpha);
	run();

	return EXIT_SUCCESS;
}

/* compatibility */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

/* the GTK assert */
#define UASSERT g_assert

#include <gtk/gtk.h>

extern "C" {
	GtkLabel *getLabelFromContainer(GtkContainer *w);
}

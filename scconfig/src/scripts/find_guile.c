/*
    scconfig - guile lib detection
    Copyright (C) 2009  Tibor Palinkas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

		Project page: http://repo.hu/projects/scconfig
		Contact via email: scconfig [at] igor2.repo.hu
*/

#include "scripts.h"
#include <unistd.h>


int find_script_guile(const char *name, int logdepth, int fatal)
{
	char *cflags, *ldflags;

/* temp hack: guile/gh makes sure we have the old, 1.8 version */
	char *test_c =
		NL "#include <stdio.h>"
		NL "#include <libguile.h>"
		NL "#include <guile/gh.h>"
		NL "int main(int argc, char *argv[]) {"
		NL "	scm_init_guile();"
		NL
		NL "	puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;

	require("cc/cc", logdepth, fatal);

	report("Checking for guile... ");
	logprintf(logdepth, "find_guile: trying to find guile...\n");
	logdepth++;

	if (run(logdepth, "guile-config compile", &cflags) + run(logdepth, "guile-config link", &ldflags) > 0) {
		free(cflags);
		free(ldflags);
		put("libs/script/guile/presents", sfalse);
		report("FAILED (guile-config failed)\n");
		return 1;
	}

	/* TODO: do we need -ldl? */
	if (try_icl(logdepth, "libs/script/guile", test_c, NULL, cflags, ldflags)) {
		free(ldflags);
		free(cflags);
		return 0;
	}

	free(ldflags);
	free(cflags);
	return try_fail(logdepth, "libs/script/guile");
}

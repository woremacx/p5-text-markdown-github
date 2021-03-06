/*
 * Copyright (c) 2012, GitHub, Inc
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "markdown.h"
#include "html.h"
#include "plaintext.h"

#include <string.h>

static struct {
	struct sd_markdown *md;
	struct html_renderopt render_opts;
} g_markdown, g_GFM, g_plaintext;

static void
rndr_blockcode_github(
	struct buf *ob,
	const struct buf *text,
	const struct buf *lang,
	void *opaque)
{
	if (ob->size)
		bufputc(ob, '\n');

	if (!text || !text->size) {
		BUFPUTSL(ob, "<pre><code></code></pre>");
		return;
	}

	if (lang && lang->size) {
		size_t i = 0, lang_size;
		const char *lang_name = NULL;

		while (i < lang->size && !isspace(lang->data[i]))
			i++;

		if (lang->data[0] == '.') {
			lang_name = lang->data + 1;
			lang_size = i - 1;
		} else {
			lang_name = lang->data;
			lang_size = i;
		}

/*
		if (rb_block_given_p()) {
			VALUE hilight;

			hilight = rb_yield_values(2,
				geefem_str_new(text->data, text->size),
				geefem_str_new(lang_name, lang_size));

			if (!NIL_P(hilight)) {
				Check_Type(hilight, T_STRING);
				bufput(ob, RSTRING_PTR(hilight), RSTRING_LEN(hilight));
				return;
			}
		}
*/

		BUFPUTSL(ob, "<pre lang=\"");
		houdini_escape_html0(ob, lang_name, lang_size, 0);
		BUFPUTSL(ob, "\"><code>");

	} else {
		BUFPUTSL(ob, "<pre><code>");
	}

	houdini_escape_html0(ob, text->data, text->size, 0);
	BUFPUTSL(ob, "</code></pre>\n");
}

/* Max recursion nesting when parsing Markdown documents */
static const int GITHUB_MD_NESTING = 32;

/* Default flags for all Markdown pipelines:
 *
 *	- NO_INTRA_EMPHASIS: disallow emphasis inside of words
 *	- LAX_SPACING: Do spacing like in Markdown 1.0.0 (i.e.
 *		do not require an empty line between two different
 *		blocks in a paragraph)
 *	- STRIKETHROUGH: strike out words with `~~`, same semantics
 *		as emphasis
 *	- TABLES: the tables extension from PHP-Markdown extra
 *	- FENCED_CODE: the fenced code blocks extension from
 *		PHP-Markdown extra, but working with ``` besides ~~~.
 *	- AUTOLINK: Well. That. Link stuff automatically.
 */
static const int GITHUB_MD_FLAGS =
	MKDEXT_NO_INTRA_EMPHASIS |
	MKDEXT_LAX_SPACING |
	MKDEXT_STRIKETHROUGH |
	MKDEXT_TABLES |
	MKDEXT_FENCED_CODE |
	MKDEXT_AUTOLINK;

/* Init the default pipeline */
static void rb_ghmd__init_md(void)
{
	struct sd_callbacks callbacks;

	/* No extra flags to the Markdown renderer */
	sdhtml_renderer(&callbacks, &g_markdown.render_opts, 0);
	callbacks.blockcode = &rndr_blockcode_github;

	g_markdown.md = sd_markdown_new(
		GITHUB_MD_FLAGS,
		GITHUB_MD_NESTING,
		&callbacks,
		&g_markdown.render_opts
	);
}

/* Init the GFM pipeline */
static void rb_ghmd__init_gfm(void)
{
	struct sd_callbacks callbacks;

	/*
	 * The following extensions to the HTML output are enabled:
	 *
	 *	- HARD_WRAP: line breaks are replaced with <br>
	 *		entities
	 */
	sdhtml_renderer(&callbacks, &g_GFM.render_opts, HTML_HARD_WRAP);
	callbacks.blockcode = &rndr_blockcode_github;

	/* The following extensions to the parser are enabled, on top
	 * of the common ones:
	 *
	 *	- SPACE_HEADERS: require a space between the `#` and the
	 *		name of a header (prevents collisions with the Issues
	 *		filter)
	 */
	g_GFM.md = sd_markdown_new(
		GITHUB_MD_FLAGS | MKDEXT_SPACE_HEADERS,
		GITHUB_MD_NESTING,
		&callbacks,
		&g_GFM.render_opts
	);
}

static void rb_ghmd__init_plaintext(void)
{
	struct sd_callbacks callbacks;

	sdtext_renderer(&callbacks);
	g_plaintext.md = sd_markdown_new(
		GITHUB_MD_FLAGS,
		GITHUB_MD_NESTING,
		&callbacks, NULL
	);
}

void github_markdown_init()
{
	rb_ghmd__init_md();
	rb_ghmd__init_gfm();
	rb_ghmd__init_plaintext();
}

int github_markdown_render(char *text, char **htmlout)
{
    char *html;
    int size;
	struct buf *output_buf;
	struct sd_markdown *md = NULL;
	//ID mode;

	if (!text)
		return -1;

	/* check for rendering mode */
#if 0
	if (mode == rb_intern("markdown")) {
		md = g_markdown.md;
	} else if (mode == rb_intern("gfm")) {
		md = g_GFM.md;
	} else if (mode == rb_intern("plaintext")) {
		md = g_plaintext.md;
	} else {
		rb_raise(rb_eTypeError, "Invalid render mode");
	}
#endif
    md = g_GFM.md;

	/* initialize buffers */
	output_buf = bufnew(128);

	/* render the magic */
	sd_markdown_render(output_buf, text, strlen(text), md);

	html = (char *)malloc(output_buf->size + 1);
	memcpy(html, output_buf->data, output_buf->size);
	html[output_buf->size] = 0;
    *htmlout = html;
	bufrelease(output_buf);

	return output_buf->size;
}

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <markdown.h>
#include <html.h>
#include <plaintext.h>
#include <gh-markdown.h>

static struct {
	struct sd_markdown *md;
	struct html_renderopt render_opts;
} g_markdown;


MODULE = Text::Markdown::Github		PACKAGE = Text::Markdown::Github	PREFIX = TextMarkdownGithub_

PROTOTYPES: DISABLE

SV *
TextMarkdownGithub__markdown(sv_str)
        SV *sv_str
    PREINIT:
        bool is_utf8 = SvUTF8(sv_str) != 0; // SvUTF8 doesn't typecast consistently to bool across various archs
        char *text = SvPV_nolen(sv_str);
        SV* r = &PL_sv_undef;
        char *html = NULL;
        int szhtml;
        int c;
    CODE:
        github_markdown_init();
        if ( (szhtml = github_markdown_render(text, &html)) == -1 ) {
            croak("github_markdown_render");
        }
        r = newSVpvn(html, szhtml);
        //sv_catpv(r, "\n");
        if (is_utf8) {
            sv_utf8_decode(r);
        }

        RETVAL = r;
    OUTPUT:
        RETVAL


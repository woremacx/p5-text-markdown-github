OBJS = autolink.o buffer.o houdini_href_e.o houdini_html_e.o html.o markdown.o plaintext.o stack.o gh-markdown.o
LIB = libgithubmarkdown.a

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs libgithubmarkdown.a $(OBJS)

.c.o:
	gcc -c -fPIC $<

clean:
	rm -f $(LIB) $(OBJS)

